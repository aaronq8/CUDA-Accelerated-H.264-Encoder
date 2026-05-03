#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <nvcuvid.h>

#include "FFmpegDemuxer.h"
#include "Logger.h"
#include "NvDecoder.h"
#include <boost/program_options.hpp>
#include <cuda.h>
#include <cuda_driver_helper.hpp>
#include <cuviddec.h>
#include <fstream>
#include <iostream>
#include <nvEncodeAPI.h>

namespace po = boost::program_options;

struct AppConfig {
  std::string input_video_file_path;
};

AppConfig parseArgs(int argc, char **argv) {
  AppConfig config;

  po::options_description desc("Allowed options");
  desc.add_options()("help,h", "Show help message")(
      "input_video_file_path,i",
      po::value<std::string>(&config.input_video_file_path)->required(),
      "Path to input video file");

  po::variables_map vm;

  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      LOG(INFO) << desc << "\n";
      std::exit(0);
    }

    po::notify(vm); // checks required args
  } catch (const po::error &e) {
    LOG(ERROR) << "Argument error: " << e.what() << "\n\n";
    LOG(ERROR) << desc << "\n";
    std::exit(1);
  }

  return config;
}

int main(int argc, char **argv) {
  LOG(INFO) << "Parsing input...";
  AppConfig config = parseArgs(argc, argv);
  LOG(INFO) << config.input_video_file_path;
  CUdevice gpu = 0;
  CUcontext cu_ctx = NULL;
  cuda::init_gpu(gpu, cu_ctx);
  // Main Decode Loop
  std::ofstream fpout("./output_frames.yuv", std::ios::out | std::ios::binary);
  FFmpegDemuxer demuxer{config.input_video_file_path.c_str()};
  auto input_vid_codec = demuxer.GetVideoCodec();
  NvDecoder dec(cu_ctx, false, FFmpeg2NvCodecId(input_vid_codec));
  int packet_sz = 0;
  uint8_t *packet = NULL, *frame;
  int num_frames_cur_packet = 0, tot_frames = 0;
  do {
    demuxer.Demux(&packet, &packet_sz);
    num_frames_cur_packet = dec.Decode(packet, packet_sz);
    if (!tot_frames && num_frames_cur_packet) {
      LOG(INFO) << dec.GetVideoInfo();
    }
    for (int i = 0; i < num_frames_cur_packet; i++) {
      frame = dec.GetFrame();
      // https: // gist.github.com/Jim-Bar/3cbba684a71d1a9d468a6711a6eddbeb
      /*
      Raw frame
      Y0 : [pixels][padding]
      Y1 : [pixels][padding]
      ....
      YH :
      UV0 : [pixels]
      UV1 : [pixels]
      ....
      UVH/2 :
      */
      YuvConverter<uint8_t> converter8(dec.GetWidth(), dec.GetHeight(),
                                       dec.GetOutputChromaFormat());
      converter8.UVInterleavedToPlanar(frame);

      // convert to greyscale
      uint8_t *chroma_section = frame + dec.GetLumaPlaneSize();
      for (int i = 0; i < dec.GetChromaHeight(); i++) {
        uint8_t *chroma_row = chroma_section + i * dec.GetWidth();
        for (int j = 0; j < dec.GetWidth(); j++) {
          chroma_row[j] = 128;
        }
      }
      // dump YUV to disk
      if (dec.GetWidth() == dec.GetDecodeWidth()) {
        fpout.write(reinterpret_cast<char *>(frame), dec.GetFrameSize());
      } else {
        // If decoded frame width is != output buffer means padding was appended
        // we need to skip last few bytes
        assert(dec.GetDecodeWidth() < dec.GetWidth());
        for (auto i = 0; i < dec.GetHeight(); i++) {
          fpout.write(reinterpret_cast<char *>(frame),
                      dec.GetDecodeWidth() * dec.GetBPP());
          // skip over Luma pixels
          frame += (dec.GetDecodeWidth() * dec.GetBPP());
          // skip over padding
          frame += ((dec.GetWidth() - dec.GetDecodeWidth()) * dec.GetBPP());
        }
        // dump Chroma
        fpout.write(reinterpret_cast<char *>(frame), dec.GetChromaPlaneSize());
      }
    }
    tot_frames += num_frames_cur_packet;
  } while (packet_sz);
  LOG(INFO) << "Total frames.. : " << tot_frames;
  fpout.close();
  // use ffplay to play the decoded raw frames
  std::system("ffplay -f rawvideo -pixel_format yuv420p -video_size 1920x1080 "
              "-framerate 30 output_frames.yuv");
  return 0;
}

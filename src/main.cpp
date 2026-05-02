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
  std::ofstream fpout("./output_frames", std::ios::out | std::ios::binary);
  FFmpegDemuxer demuxer{config.input_video_file_path.c_str()};
  auto input_vid_codec = demuxer.GetVideoCodec();
  Rect crop_rect{};
  Dim resize_dim{};
  NvDecoder dec(cu_ctx, false, FFmpeg2NvCodecId(input_vid_codec));
  return 0;
}

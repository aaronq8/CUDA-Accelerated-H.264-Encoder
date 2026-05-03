# CUDA-Accelerated-H.264-Encoder
POC for CUDA accelerated Transcoding using H.264 Codec
- Decoding works..verified using ffplay
- Currently supports only 8 bit YUV 4:2:0
- Added Greyscale filter for testing
# Build
```sh build.sh```
# Logs 
```text
aaroncp@aaroncp-ROG-Strix-G634JZ-G634JZ:~/Desktop/workspace/CUDA-Accelerated-H.264-Encoder$ ./build/encoder -i ./test/sample_h264.mp4 
[INFO ][02:13:53] Parsing input...
[INFO ][02:13:53] ./test/sample_h264.mp4
[INFO ][02:13:53] cuda initialised..
[INFO ][02:13:53] Running on GPU : NVIDIA GeForce RTX 4080 Laptop GPU
[INFO ][02:13:53] Initialised CU Context...
[INFO ][02:13:53] Media format: QuickTime / MOV (mov,mp4,m4a,3gp,3g2,mj2)
Session Deinitialization Time: 0 ms 
```
# Testing
Verified correctness of raw YUV frames using ffplay
```ffplay -f rawvideo -pixel_format yuv420p -video_size 1920x1080 -framerate 30 output_frames.yuv```


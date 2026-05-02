#pragma once
#include <Logger.h>
#include <cuda.h>

namespace cuda {
static void ck(CUresult result) {
  if (result != CUDA_SUCCESS) {
    const char *name = nullptr;
    const char *msg = nullptr;

    cuGetErrorName(result, &name);
    cuGetErrorString(result, &msg);

    LOG(ERROR) << "CUDA error: " << (name ? name : "unknown") << " - "
               << (msg ? msg : "unknown") << "\n";

    std::exit(EXIT_FAILURE);
  }
}

static void init_gpu(CUdevice &gpu) {
  cuda::ck(cuInit(0));
  LOG(INFO) << "cuda initialised..";
  // assign device
  cuda::ck(cuDeviceGet(&gpu, 0));

  char gpu_name[100];
  cuda::ck(cuDeviceGetName(gpu_name, sizeof(gpu_name), gpu));
  LOG(INFO) << "Running on GPU : " << gpu_name;
}

}; // namespace cuda
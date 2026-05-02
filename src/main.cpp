#include "Logger.h"
#include <cuda.h>
#include <cuda_driver_helper.hpp>
#include <cuviddec.h>
#include <nvEncodeAPI.h>
#include <nvcuvid.h>

int main(int, char **) {
  LOG(INFO) << "Program started";
  CUdevice gpu = 0;
  cuda::init_gpu(gpu);
  return 0;
}

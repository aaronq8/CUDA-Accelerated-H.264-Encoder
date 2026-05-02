cmake_minimum_required(VERSION 3.10.0)
project(encoder VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VIDEO_CODEC_SDK_ROOT
    "${CMAKE_SOURCE_DIR}/third_party/Video_Codec_SDK_13.0.37"
)

set(VIDEO_CODEC_SDK_INTERFACE_DIR
    "${VIDEO_CODEC_SDK_ROOT}/Interface"
)

set(NV_CODEC_SAMPLES_DIR
    "${VIDEO_CODEC_SDK_ROOT}/Samples"
)

set(NV_CODEC_NVCODEC_DIR
    "${NV_CODEC_SAMPLES_DIR}/NvCodec"
)

#find cmake targets of NVENC and NVDEC
find_library(NVIDIA_ENCODE_LIB
    NAMES nvidia-encode
    REQUIRED
)

find_library(NVCUVID_LIB
    NAMES nvcuvid
    REQUIRED
)

find_library(CUDA_DRIVER_LIB
    NAMES cuda
    REQUIRED
)

find_package(CUDAToolkit REQUIRED)


add_executable(encoder 
    src/main.cpp
    src/nvcodec_logger.cpp
    "${NV_CODEC_NVCODEC_DIR}/NvDecoder/NvDecoder.cpp" # for NvDecoder helper
)

target_include_directories(encoder PRIVATE
    ${VIDEO_CODEC_SDK_INTERFACE_DIR}
    "${NV_CODEC_NVCODEC_DIR}"
    "${NV_CODEC_NVCODEC_DIR}/NvDecoder"
    "${NV_CODEC_SAMPLES_DIR}/Utils"
    ${CUDAToolkit_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(encoder PRIVATE
    ${NVIDIA_ENCODE_LIB}
    ${NVCUVID_LIB}
    ${CUDA_DRIVER_LIB}
    dl
    pthread
)
project(deep-robomaster)

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_definitions(-std=c++14)

include_directories(
    #OpenVINO推理引擎的头文件
    /opt/intel/openvino/runtime/include
    /home/robomaster/demo_opencv
)


set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} hello_onnx.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
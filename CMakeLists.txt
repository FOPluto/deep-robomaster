project(deep-robomaster)

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CMAKE_VERBOSE_MAKEFILE ON)

# openvino
set(CMAKE_PREFIX_PATH "/opt/intel/openvino/")

set(CMAKE_CXX_STANDARD 14)

find_package(InferenceEngine REQUIRED)

include_directories(/home/robomaster/demo_opencv ${InferenceEngine_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} demo.cpp Yolov5.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} ${InferenceEngine_LIBRARIES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
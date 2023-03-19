project(demo_opencv)

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_definitions(-std=c++14)

set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(OpenCV REQUIRED)

include(FetchContent)
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG        v2.6.2
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(pybind11)

add_executable(demo_opencv demo.cpp)
include_directories(/home/robomaster/opencv_home/deep-robomaster)

target_link_libraries(demo_opencv PRIVATE ${OpenCV_LIBS} pybind11::embed)

set_property(TARGET demo_opencv PROPERTY CXX_STANDARD 14)

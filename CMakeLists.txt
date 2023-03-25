cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

project(demo_opencv)

add_definitions(-std=c++14)

set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} demo.cpp)

set(TORCH_DIR /home/fo_pluto/libtorch)

find_package(Torch REQUIRED
            NO_MODULE
            PATHS /home/fo_pluto/libtorch
            NO_DEFAULT_PATH)

include_directories(/home/fo_pluto/demo_opencv ${TORCH_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} ${TORCH_LIBRARIES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
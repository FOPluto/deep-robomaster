cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(OPENVINO_DIR, /opt/intel)

project(demo_opencv)

add_definitions(-std=c++14)

find_package(OpenCV REQUIRED)

find_package(OPENVINO REQUIRED)

add_executable(${PROJECT_NAME} demo.cpp /home/fo_pluto/${PROJECT_NAME}/src/Yolov5.cpp)
   
include_directories(/home/fo_pluto/${PROJECT_NAME}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
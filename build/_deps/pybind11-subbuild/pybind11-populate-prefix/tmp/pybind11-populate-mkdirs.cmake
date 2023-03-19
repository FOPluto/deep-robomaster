# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/robomaster/opencv_home/deep-robomaster/build/_deps/pybind11-src"
  "/home/robomaster/opencv_home/deep-robomaster/build/_deps/pybind11-build"
  "/home/robomaster/opencv_home/deep-robomaster/build/_deps/pybind11-subbuild/pybind11-populate-prefix"
  "/home/robomaster/opencv_home/deep-robomaster/build/_deps/pybind11-subbuild/pybind11-populate-prefix/tmp"
  "/home/robomaster/opencv_home/deep-robomaster/build/_deps/pybind11-subbuild/pybind11-populate-prefix/src/pybind11-populate-stamp"
  "/home/robomaster/opencv_home/deep-robomaster/build/_deps/pybind11-subbuild/pybind11-populate-prefix/src"
  "/home/robomaster/opencv_home/deep-robomaster/build/_deps/pybind11-subbuild/pybind11-populate-prefix/src/pybind11-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/robomaster/opencv_home/deep-robomaster/build/_deps/pybind11-subbuild/pybind11-populate-prefix/src/pybind11-populate-stamp/${subDir}")
endforeach()

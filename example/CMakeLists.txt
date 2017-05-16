cmake_minimum_required(VERSION 2.8)
project(injection)
include_directories(../ Panku)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -std=c++14")
add_executable(main main.cpp Devices.cpp ExampleDeviceClasses.cpp)

cmake_minimum_required(VERSION 2.8)
project(injection)
include_directories(../ Panku)

add_executable(main main.cpp Devices.cpp ExampleDeviceClasses.cpp)

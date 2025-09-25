cmake_minimum_required(VERSION 3.16)
project(pi_camera_streamer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Add include directory
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/capture.cpp
    src/encoder.cpp
    src/sender.cpp
)

add_executable(pi-camera-streamer ${SOURCES})

cmake_minimum_required(VERSION 3.16)
project(pi_camera_streamer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Find spdlog (installed via apt or vcpkg)
find_package(spdlog REQUIRED)

# Include project headers
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/capture.cpp
    src/encoder.cpp
    src/sender.cpp
)

# Executable
add_executable(pi-camera-streamer ${SOURCES})

# Link dependencies
target_link_libraries(pi-camera-streamer
    PRIVATE
        spdlog::spdlog
        Threads::Threads
)

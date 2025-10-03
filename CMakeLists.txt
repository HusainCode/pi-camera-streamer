cmake_minimum_required(VERSION 3.16)
project(pi_camera_streamer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------
# Dependencies
# ----------------------------------------
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)

# Include project headers
include_directories(include)

# ----------------------------------------
# Main Executable
# ----------------------------------------
set(SOURCES
    src/main.cpp
    src/capture.cpp
    src/encoder.cpp
    src/sender.cpp
    src/buffer.cpp
    src/frame.cpp
    src/logger.cpp
)

add_executable(pi-camera-streamer ${SOURCES})

target_link_libraries(pi-camera-streamer
    PRIVATE
        spdlog::spdlog
        Threads::Threads
)

# ----------------------------------------
# GoogleTest Setup
# ----------------------------------------
include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.zip
)
# Avoid overriding compiler/linker options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ----------------------------------------
# Test Executable
# ----------------------------------------
# Collect all test files under test/
file(GLOB TEST_SOURCES test/*.cpp)

add_executable(pi-camera-tests
    ${TEST_SOURCES}
)

target_link_libraries(pi-camera-tests
    PRIVATE
        gtest_main
        spdlog::spdlog
        Threads::Threads
)

# Auto-discover tests
include(GoogleTest)
gtest_discover_tests(pi-camera-tests)

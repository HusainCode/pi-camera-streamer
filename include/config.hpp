#pragma once

#include <string>

namespace config {

// Configuration structure for the camera streamer
struct CameraConfig {
    int width = 640;
    int height = 480;
    int fps = 30;
    std::string device = "/dev/video0";
    std::string stream_url = "rtmp://localhost/live/stream";
};

// Loads configuration from a file (returns default if not found)
CameraConfig load_config(const std::string& path);

} // namespace config
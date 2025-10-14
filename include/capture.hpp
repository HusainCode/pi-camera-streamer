#pragma once

#include <opencv2/opencv.hpp>
#include <memory>

class Capture {
public:
    Capture(int deviceID = 0, int width = 640, int height = 480);
    ~Capture();

    bool isOpened() const;
    bool read(cv::Mat& frame);
    void setResolution(int width, int height);

private:
    std::unique_ptr<cv::VideoCapture> cap_;
    int width_;
    int height_;
};
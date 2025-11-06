#include "capture.hpp"
#include <iostream>

Capture::Capture(int deviceID, int width, int height)
    : width_(width), height_(height), cap_(std::make_unique<cv::VideoCapture>(deviceID)) 
{
    if (!cap_->isOpened()) {
        std::cerr << "❌ Error: Could not open video device " << deviceID << std::endl;
        return;
    }

    cap_->set(cv::CAP_PROP_FRAME_WIDTH, width_);
    cap_->set(cv::CAP_PROP_FRAME_HEIGHT, height_);

    std::cout << "✅ Camera initialized at " << width_ << "x" << height_ << std::endl;
}

Capture::~Capture() {
    if (cap_ && cap_->isOpened()) {
        cap_->release();
        std::cout << "📷 Camera released." << std::endl;
    }
}

bool Capture::isOpened() const {
    return cap_ && cap_->isOpened();
}

bool Capture::read(cv::Mat& frame) {
    if (!isOpened()) return false;
    return cap_->read(frame);
}

void Capture::setResolution(int width, int height) {
    width_ = width;
    height_ = height;

    if (isOpened()) {
        cap_->set(cv::CAP_PROP_FRAME_WIDTH, width_);
        cap_->set(cv::CAP_PROP_FRAME_HEIGHT, height_);
    }
}

#include "frame.hpp"
#include <utility>   // for std::move
#include <sstream>   // for toString()
#include <iomanip>   // for timestamp formatting

using Clock = std::chrono::steady_clock;

// Default: empty frame with current timestamp
Frame::Frame()
    : m_data(),
      m_width(0),
      m_height(0),
      m_channels(0),
      m_timestamp(Clock::now()) {}

// Construct a frame with image data and metadata
Frame::Frame(std::vector<uint8_t> data, uint32_t width, uint32_t height, uint32_t channels)
    : m_data(std::move(data)),
      m_width(width),
      m_height(height),
      m_channels(channels),
      m_timestamp(Clock::now()) {}

// Replace pixel data (zero-copy move)
void Frame::setData(std::vector<uint8_t> data)
{
    m_data = std::move(data);
    m_timestamp = Clock::now();
}

// Refresh timestamp to current time
void Frame::setTimestampNow() noexcept
{
    m_timestamp = Clock::now();
}

// Deep copy clone (used when encoder/sender need ownership)
Frame Frame::clone() const
{
    Frame copy;
    copy.m_data = m_data;
    copy.m_width = m_width;
    copy.m_height = m_height;
    copy.m_channels = m_channels;
    copy.m_timestamp = m_timestamp;
    return copy;
}

// Return age of frame in milliseconds (for latency metrics)
uint64_t Frame::ageMs() const noexcept
{
    auto now = Clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_timestamp).count();
}

// Debug utility for logging
std::string Frame::toString() const
{
    std::ostringstream oss;
    oss << "Frame(" << m_width << "x" << m_height
        << "x" << m_channels
        << ", bytes=" << m_data.size()
        << ", age=" << ageMs() << "ms)";
    return oss.str();
}

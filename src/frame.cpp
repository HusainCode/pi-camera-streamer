#include "frame.hpp"
#include <utility>   // for std::move, std::swap
#include <sstream>   // for toString()
#include <iomanip>   // for timestamp formatting
#include <algorithm> // for std::swap

using Clock = std::chrono::steady_clock;

// ============================================================================
// Constructors (Rule of 5)
// ============================================================================

// Default: empty frame with current timestamp
Frame::Frame() noexcept
    : m_data(),
      m_width(0),
      m_height(0),
      m_channels(0),
      m_timestamp(Clock::now()) {}

// Construct a frame with image data and metadata (move-optimized)
Frame::Frame(std::vector<uint8_t> data, uint32_t width, uint32_t height, uint32_t channels) noexcept
    : m_data(std::move(data)),
      m_width(width),
      m_height(height),
      m_channels(channels),
      m_timestamp(Clock::now()) {}

// Copy constructor (deep copy)
Frame::Frame(const Frame& other)
    : m_data(other.m_data),
      m_width(other.m_width),
      m_height(other.m_height),
      m_channels(other.m_channels),
      m_timestamp(other.m_timestamp) {}

// Move constructor (zero-copy, noexcept for optimization)
Frame::Frame(Frame&& other) noexcept
    : m_data(std::move(other.m_data)),
      m_width(other.m_width),
      m_height(other.m_height),
      m_channels(other.m_channels),
      m_timestamp(other.m_timestamp)
{
    // Reset moved-from object to valid state
    other.m_width = 0;
    other.m_height = 0;
    other.m_channels = 0;
}

// Copy assignment
Frame& Frame::operator=(const Frame& other)
{
    if (this != &other) {
        m_data = other.m_data;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;
        m_timestamp = other.m_timestamp;
    }
    return *this;
}

// Move assignment (zero-copy, noexcept for optimization)
Frame& Frame::operator=(Frame&& other) noexcept
{
    if (this != &other) {
        m_data = std::move(other.m_data);
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;
        m_timestamp = other.m_timestamp;

        // Reset moved-from object
        other.m_width = 0;
        other.m_height = 0;
        other.m_channels = 0;
    }
    return *this;
}

// ============================================================================
// Mutators
// ============================================================================

// Replace pixel data (zero-copy move)
void Frame::setData(std::vector<uint8_t> data) noexcept
{
    m_data = std::move(data);
    m_timestamp = Clock::now();
}

// Refresh timestamp to current time
void Frame::setTimestampNow() noexcept
{
    m_timestamp = Clock::now();
}

// Update frame dimensions
void Frame::setDimensions(uint32_t width, uint32_t height, uint32_t channels) noexcept
{
    m_width = width;
    m_height = height;
    m_channels = channels;
}

// Pre-allocate buffer to avoid reallocations during capture
void Frame::reserve(size_t capacity)
{
    m_data.reserve(capacity);
}

// ============================================================================
// Utilities
// ============================================================================

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

// Return age of frame in microseconds (for high-precision latency metrics)
uint64_t Frame::ageUs() const noexcept
{
    auto now = Clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - m_timestamp).count();
}

// Debug utility for logging
std::string Frame::toString() const
{
    std::ostringstream oss;
    oss << "Frame(" << m_width << "x" << m_height
        << "x" << m_channels
        << ", bytes=" << m_data.size()
        << ", expected=" << expectedSize()
        << ", valid=" << (isValid() ? "yes" : "no")
        << ", age=" << ageMs() << "ms)";
    return oss.str();
}

// Efficient swap operation (noexcept for strong exception guarantee)
void Frame::swap(Frame& other) noexcept
{
    using std::swap;  // ADL
    swap(m_data, other.m_data);
    swap(m_width, other.m_width);
    swap(m_height, other.m_height);
    swap(m_channels, other.m_channels);
    swap(m_timestamp, other.m_timestamp);
}

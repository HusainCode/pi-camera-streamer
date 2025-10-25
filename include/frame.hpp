#pragma once

#include <vector>
#include <cstdint>
#include <chrono>
#include <string>

/**
 * @brief Represents a single image frame captured from the camera.
 *
 * A Frame stores the raw pixel data along with basic metadata:
 * width, height, channel count, and a precise capture timestamp.
 *
 * Frames are lightweight, movable objects that can be passed efficiently
 * between the capture, encoder, and network sender threads without copying.
 * Each frame is timestamped using a steady clock, enabling accurate
 * latency measurement and synchronization across the pipeline.
 */

class Frame
{
public:
    using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;

    // --- Constructors ---
    Frame();  // Empty frame
    Frame(std::vector<uint8_t> data, uint32_t width, uint32_t height, uint32_t channels);

    // --- Accessors ---
    const std::vector<uint8_t>& data() const noexcept { return m_data; }
    uint32_t width() const noexcept { return m_width; }
    uint32_t height() const noexcept { return m_height; }
    uint32_t channels() const noexcept { return m_channels; }
    Timestamp timestamp() const noexcept { return m_timestamp; }

    // --- Mutators ---
    void setData(std::vector<uint8_t> data);
    void setTimestampNow() noexcept;

    // --- Utilities ---
    size_t size() const noexcept { return m_data.size(); }
    bool empty() const noexcept { return m_data.empty(); }

    Frame clone() const;              // Deep copy
    uint64_t ageMs() const noexcept;  // Age since capture in ms
    std::string toString() const;     // Human-readable debug info

private:
    std::vector<uint8_t> m_data;
    uint32_t m_width{0};
    uint32_t m_height{0};
    uint32_t m_channels{0};
    Timestamp m_timestamp;
};

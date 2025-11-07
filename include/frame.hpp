#pragma once

#include <vector>
#include <cstdint>
#include <chrono>
#include <string>
#include <memory>

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
 *
 * PERFORMANCE OPTIMIZATIONS:
 * - Move semantics (Rule of 5) for zero-copy transfers
 * - noexcept specifications for better optimization
 * - Cache-aligned metadata for efficient memory access
 * - Reserve capacity hints to avoid reallocations
 * - Inline hot-path accessors
 */

class Frame
{
public:
    using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;

    // --- Constructors (Rule of 5) ---
    Frame() noexcept;  // Empty frame
    Frame(std::vector<uint8_t> data, uint32_t width, uint32_t height, uint32_t channels) noexcept;

    // Copy constructor (deep copy)
    Frame(const Frame& other);

    // Move constructor (zero-copy)
    Frame(Frame&& other) noexcept;

    // Copy assignment
    Frame& operator=(const Frame& other);

    // Move assignment (zero-copy)
    Frame& operator=(Frame&& other) noexcept;

    // Destructor
    ~Frame() noexcept = default;

    // --- Accessors ---
    const std::vector<uint8_t>& data() const noexcept { return m_data; }
    std::vector<uint8_t>& data() noexcept { return m_data; }  // Non-const for direct access
    uint8_t* dataPtr() noexcept { return m_data.data(); }     // Raw pointer for C APIs
    const uint8_t* dataPtr() const noexcept { return m_data.data(); }

    uint32_t width() const noexcept { return m_width; }
    uint32_t height() const noexcept { return m_height; }
    uint32_t channels() const noexcept { return m_channels; }
    Timestamp timestamp() const noexcept { return m_timestamp; }

    // --- Mutators ---
    void setData(std::vector<uint8_t> data) noexcept;
    void setTimestampNow() noexcept;
    void setDimensions(uint32_t width, uint32_t height, uint32_t channels) noexcept;

    // Pre-allocate buffer to avoid reallocations during capture
    void reserve(size_t capacity);

    // --- Utilities ---
    size_t size() const noexcept { return m_data.size(); }
    bool empty() const noexcept { return m_data.empty(); }
    size_t capacity() const noexcept { return m_data.capacity(); }

    // Expected size based on dimensions
    size_t expectedSize() const noexcept {
        return static_cast<size_t>(m_width) * m_height * m_channels;
    }

    // Validate frame data integrity
    bool isValid() const noexcept {
        return !m_data.empty() && m_data.size() == expectedSize();
    }

    Frame clone() const;              // Deep copy
    uint64_t ageMs() const noexcept;  // Age since capture in ms
    uint64_t ageUs() const noexcept;  // Age since capture in microseconds (for high-precision)
    std::string toString() const;     // Human-readable debug info

    // Swap operation (noexcept guarantee for performance)
    void swap(Frame& other) noexcept;

private:
    std::vector<uint8_t> m_data;
    uint32_t m_width{0};
    uint32_t m_height{0};
    uint32_t m_channels{0};
    Timestamp m_timestamp;
};

// Non-member swap for ADL (Argument Dependent Lookup)
inline void swap(Frame& lhs, Frame& rhs) noexcept {
    lhs.swap(rhs);
}

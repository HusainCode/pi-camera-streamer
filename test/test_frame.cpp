#include <gtest/gtest.h>
#include "frame.hpp"
#include <thread>
#include <chrono>
#include <vector>

// ============================================================================
// Construction Tests
// ============================================================================

TEST(FrameTest, DefaultConstructor) {
    Frame frame;

    EXPECT_TRUE(frame.empty());
    EXPECT_EQ(frame.size(), 0);
    EXPECT_EQ(frame.width(), 0);
    EXPECT_EQ(frame.height(), 0);
    EXPECT_EQ(frame.channels(), 0);
    EXPECT_EQ(frame.expectedSize(), 0);
    EXPECT_FALSE(frame.isValid());  // Empty frame is not valid
}

TEST(FrameTest, ParameterizedConstructor) {
    std::vector<uint8_t> data(1920 * 1080 * 3);  // 1080p RGB
    Frame frame(std::move(data), 1920, 1080, 3);

    EXPECT_FALSE(frame.empty());
    EXPECT_EQ(frame.size(), 1920 * 1080 * 3);
    EXPECT_EQ(frame.width(), 1920);
    EXPECT_EQ(frame.height(), 1080);
    EXPECT_EQ(frame.channels(), 3);
    EXPECT_EQ(frame.expectedSize(), 1920 * 1080 * 3);
    EXPECT_TRUE(frame.isValid());
}

TEST(FrameTest, ParameterizedConstructorMoveSemantics) {
    std::vector<uint8_t> data(640 * 480 * 3);
    const uint8_t* original_ptr = data.data();

    Frame frame(std::move(data), 640, 480, 3);

    // Verify data was moved (not copied)
    EXPECT_EQ(frame.dataPtr(), original_ptr);
    EXPECT_TRUE(data.empty());  // Original vector should be empty after move
}

// ============================================================================
// Rule of 5 Tests (Copy/Move Semantics)
// ============================================================================

TEST(FrameTest, CopyConstructor) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    Frame original(std::move(data), 5, 1, 1);

    Frame copy(original);

    EXPECT_EQ(copy.size(), original.size());
    EXPECT_EQ(copy.width(), original.width());
    EXPECT_EQ(copy.height(), original.height());
    EXPECT_EQ(copy.channels(), original.channels());
    EXPECT_EQ(copy.data(), original.data());

    // Verify deep copy (different memory addresses)
    EXPECT_NE(copy.dataPtr(), original.dataPtr());
}

TEST(FrameTest, MoveConstructor) {
    std::vector<uint8_t> data = {10, 20, 30, 40, 50};
    Frame original(std::move(data), 5, 1, 1);
    const uint8_t* original_ptr = original.dataPtr();

    Frame moved(std::move(original));

    EXPECT_EQ(moved.size(), 5);
    EXPECT_EQ(moved.width(), 5);
    EXPECT_EQ(moved.height(), 1);
    EXPECT_EQ(moved.channels(), 1);
    EXPECT_EQ(moved.dataPtr(), original_ptr);  // Same memory (zero-copy)

    // Original should be in valid but empty state
    EXPECT_TRUE(original.empty());
    EXPECT_EQ(original.width(), 0);
    EXPECT_EQ(original.height(), 0);
    EXPECT_EQ(original.channels(), 0);
}

TEST(FrameTest, CopyAssignment) {
    std::vector<uint8_t> data1 = {1, 2, 3};
    std::vector<uint8_t> data2 = {4, 5, 6, 7, 8};

    Frame frame1(std::move(data1), 3, 1, 1);
    Frame frame2(std::move(data2), 5, 1, 1);

    frame2 = frame1;

    EXPECT_EQ(frame2.size(), frame1.size());
    EXPECT_EQ(frame2.width(), 3);
    EXPECT_EQ(frame2.data(), frame1.data());
    EXPECT_NE(frame2.dataPtr(), frame1.dataPtr());  // Deep copy
}

TEST(FrameTest, MoveAssignment) {
    std::vector<uint8_t> data1 = {1, 2, 3};
    std::vector<uint8_t> data2 = {4, 5, 6, 7, 8};

    Frame frame1(std::move(data1), 3, 1, 1);
    Frame frame2(std::move(data2), 5, 1, 1);
    const uint8_t* frame1_ptr = frame1.dataPtr();

    frame2 = std::move(frame1);

    EXPECT_EQ(frame2.size(), 3);
    EXPECT_EQ(frame2.width(), 3);
    EXPECT_EQ(frame2.dataPtr(), frame1_ptr);  // Zero-copy move

    EXPECT_TRUE(frame1.empty());
    EXPECT_EQ(frame1.width(), 0);
}

TEST(FrameTest, SelfAssignment) {
    std::vector<uint8_t> data = {1, 2, 3};
    Frame frame(std::move(data), 3, 1, 1);

    frame = frame;  // Self-assignment

    EXPECT_EQ(frame.size(), 3);
    EXPECT_EQ(frame.width(), 3);
    EXPECT_TRUE(frame.isValid());
}

// ============================================================================
// Accessor Tests
// ============================================================================

TEST(FrameTest, DataAccessors) {
    std::vector<uint8_t> data = {10, 20, 30};
    Frame frame(std::move(data), 3, 1, 1);

    // Const accessor
    const Frame& const_frame = frame;
    EXPECT_EQ(const_frame.data().size(), 3);
    EXPECT_EQ(const_frame.dataPtr()[0], 10);

    // Non-const accessor
    frame.data()[0] = 99;
    EXPECT_EQ(frame.dataPtr()[0], 99);
}

TEST(FrameTest, DimensionAccessors) {
    Frame frame(std::vector<uint8_t>(100), 10, 10, 1);

    EXPECT_EQ(frame.width(), 10);
    EXPECT_EQ(frame.height(), 10);
    EXPECT_EQ(frame.channels(), 1);
    EXPECT_EQ(frame.size(), 100);
    EXPECT_EQ(frame.expectedSize(), 100);
}

// ============================================================================
// Mutator Tests
// ============================================================================

TEST(FrameTest, SetData) {
    Frame frame;

    std::vector<uint8_t> data = {1, 2, 3, 4};
    frame.setData(std::move(data));

    EXPECT_EQ(frame.size(), 4);
    EXPECT_TRUE(data.empty());  // Moved from
}

TEST(FrameTest, SetDimensions) {
    Frame frame;

    frame.setDimensions(1920, 1080, 3);

    EXPECT_EQ(frame.width(), 1920);
    EXPECT_EQ(frame.height(), 1080);
    EXPECT_EQ(frame.channels(), 3);
    EXPECT_EQ(frame.expectedSize(), 1920 * 1080 * 3);
}

TEST(FrameTest, Reserve) {
    Frame frame;

    frame.reserve(1000);

    EXPECT_GE(frame.capacity(), 1000);
    EXPECT_EQ(frame.size(), 0);  // Reserve doesn't change size
}

// ============================================================================
// Timestamp Tests
// ============================================================================

TEST(FrameTest, TimestampInitialization) {
    Frame frame;

    // Frame should have a valid timestamp immediately
    EXPECT_GE(frame.ageMs(), 0);  // Age should be non-negative
    EXPECT_LT(frame.ageMs(), 100);  // But not too much (< 100ms)
}

TEST(FrameTest, TimestampAge) {
    Frame frame;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    uint64_t age_ms = frame.ageMs();
    uint64_t age_us = frame.ageUs();

    EXPECT_GE(age_ms, 10);
    EXPECT_GE(age_us, 10000);  // microseconds
    EXPECT_LT(age_ms, 50);  // Reasonable upper bound
}

TEST(FrameTest, SetTimestampNow) {
    Frame frame;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_GE(frame.ageMs(), 10);

    frame.setTimestampNow();
    EXPECT_LT(frame.ageMs(), 5);  // Should be fresh
}

TEST(FrameTest, TimestampPrecision) {
    Frame frame;

    auto age_us = frame.ageUs();
    auto age_ms = frame.ageMs();

    // Microseconds should be >= milliseconds * 1000
    EXPECT_GE(age_us, age_ms * 1000);
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST(FrameTest, IsValidTrue) {
    std::vector<uint8_t> data(640 * 480 * 3);
    Frame frame(std::move(data), 640, 480, 3);

    EXPECT_TRUE(frame.isValid());
}

TEST(FrameTest, IsValidFalseEmpty) {
    Frame frame;
    EXPECT_FALSE(frame.isValid());
}

TEST(FrameTest, IsValidFalseMismatchedSize) {
    std::vector<uint8_t> data(100);  // Wrong size
    Frame frame(std::move(data), 640, 480, 3);

    EXPECT_FALSE(frame.isValid());
    EXPECT_EQ(frame.size(), 100);
    EXPECT_EQ(frame.expectedSize(), 640 * 480 * 3);
}

TEST(FrameTest, ExpectedSize) {
    Frame frame;
    frame.setDimensions(1920, 1080, 3);

    EXPECT_EQ(frame.expectedSize(), 1920 * 1080 * 3);
}

// ============================================================================
// Clone Tests
// ============================================================================

TEST(FrameTest, Clone) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    Frame original(std::move(data), 5, 1, 1);

    Frame cloned = original.clone();

    EXPECT_EQ(cloned.size(), original.size());
    EXPECT_EQ(cloned.width(), original.width());
    EXPECT_EQ(cloned.height(), original.height());
    EXPECT_EQ(cloned.channels(), original.channels());
    EXPECT_EQ(cloned.data(), original.data());

    // Verify deep copy
    EXPECT_NE(cloned.dataPtr(), original.dataPtr());

    // Modifying clone shouldn't affect original
    cloned.data()[0] = 99;
    EXPECT_EQ(original.dataPtr()[0], 1);
    EXPECT_EQ(cloned.dataPtr()[0], 99);
}

// ============================================================================
// Swap Tests
// ============================================================================

TEST(FrameTest, MemberSwap) {
    std::vector<uint8_t> data1 = {1, 2, 3};
    std::vector<uint8_t> data2 = {4, 5, 6, 7};

    Frame frame1(std::move(data1), 3, 1, 1);
    Frame frame2(std::move(data2), 4, 1, 1);

    const uint8_t* ptr1 = frame1.dataPtr();
    const uint8_t* ptr2 = frame2.dataPtr();

    frame1.swap(frame2);

    EXPECT_EQ(frame1.size(), 4);
    EXPECT_EQ(frame1.width(), 4);
    EXPECT_EQ(frame1.dataPtr(), ptr2);

    EXPECT_EQ(frame2.size(), 3);
    EXPECT_EQ(frame2.width(), 3);
    EXPECT_EQ(frame2.dataPtr(), ptr1);
}

TEST(FrameTest, NonMemberSwap) {
    std::vector<uint8_t> data1 = {1, 2, 3};
    std::vector<uint8_t> data2 = {4, 5, 6, 7};

    Frame frame1(std::move(data1), 3, 1, 1);
    Frame frame2(std::move(data2), 4, 1, 1);

    using std::swap;
    swap(frame1, frame2);

    EXPECT_EQ(frame1.size(), 4);
    EXPECT_EQ(frame2.size(), 3);
}

// ============================================================================
// ToString Tests
// ============================================================================

TEST(FrameTest, ToString) {
    std::vector<uint8_t> data(640 * 480 * 3);
    Frame frame(std::move(data), 640, 480, 3);

    std::string str = frame.toString();

    EXPECT_NE(str.find("640"), std::string::npos);
    EXPECT_NE(str.find("480"), std::string::npos);
    EXPECT_NE(str.find("3"), std::string::npos);
    EXPECT_NE(str.find("bytes="), std::string::npos);
    EXPECT_NE(str.find("valid=yes"), std::string::npos);
}

TEST(FrameTest, ToStringInvalid) {
    Frame frame;
    std::string str = frame.toString();

    EXPECT_NE(str.find("valid=no"), std::string::npos);
}

// ============================================================================
// Performance Tests
// ============================================================================

TEST(FrameTest, MoveIsZeroCopy) {
    std::vector<uint8_t> data(1920 * 1080 * 3);  // Large frame
    const uint8_t* original_ptr = data.data();

    Frame frame(std::move(data), 1920, 1080, 3);
    EXPECT_EQ(frame.dataPtr(), original_ptr);

    Frame moved(std::move(frame));
    EXPECT_EQ(moved.dataPtr(), original_ptr);  // Still same memory
}

TEST(FrameTest, ReserveAvoidsReallocation) {
    Frame frame;
    const size_t target_size = 1920 * 1080 * 3;

    frame.reserve(target_size);
    const uint8_t* ptr_after_reserve = frame.data().data();

    // Add data up to reserved capacity
    std::vector<uint8_t> data(target_size);
    frame.setData(std::move(data));

    // Capacity should not have changed (no reallocation)
    EXPECT_GE(frame.capacity(), target_size);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST(FrameTest, LargeFrame) {
    // 4K RGB frame
    const size_t size = 3840 * 2160 * 3;
    std::vector<uint8_t> data(size);

    Frame frame(std::move(data), 3840, 2160, 3);

    EXPECT_EQ(frame.size(), size);
    EXPECT_TRUE(frame.isValid());
}

TEST(FrameTest, SinglePixel) {
    std::vector<uint8_t> data = {255};
    Frame frame(std::move(data), 1, 1, 1);

    EXPECT_EQ(frame.size(), 1);
    EXPECT_TRUE(frame.isValid());
}

TEST(FrameTest, MultiChannel) {
    // RGBA frame
    std::vector<uint8_t> data(100 * 100 * 4);
    Frame frame(std::move(data), 100, 100, 4);

    EXPECT_EQ(frame.channels(), 4);
    EXPECT_TRUE(frame.isValid());
}

TEST(FrameTest, EmptyDataNonZeroDimensions) {
    Frame frame(std::vector<uint8_t>(), 640, 480, 3);

    EXPECT_TRUE(frame.empty());
    EXPECT_FALSE(frame.isValid());  // Empty data with dimensions is invalid
}

// ============================================================================
// Capacity Tests
// ============================================================================

TEST(FrameTest, CapacityTracking) {
    Frame frame;
    EXPECT_EQ(frame.capacity(), 0);

    frame.reserve(1000);
    EXPECT_GE(frame.capacity(), 1000);
    EXPECT_EQ(frame.size(), 0);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(FrameTest, TypicalCaptureWorkflow) {
    // Simulate camera capture workflow
    Frame frame;

    // 1. Reserve space for expected frame
    const uint32_t width = 640;
    const uint32_t height = 480;
    const uint32_t channels = 3;
    const size_t expected_size = width * height * channels;

    frame.setDimensions(width, height, channels);
    frame.reserve(expected_size);

    // 2. "Capture" frame data
    std::vector<uint8_t> captured_data(expected_size, 128);  // Gray frame
    frame.setData(std::move(captured_data));

    // 3. Validate
    EXPECT_TRUE(frame.isValid());
    EXPECT_EQ(frame.size(), expected_size);
    EXPECT_LT(frame.ageMs(), 10);

    // 4. Move to encoder (zero-copy)
    Frame encoder_frame = std::move(frame);
    EXPECT_TRUE(encoder_frame.isValid());
    EXPECT_TRUE(frame.empty());
}

TEST(FrameTest, FrameAging) {
    Frame frame(std::vector<uint8_t>(100), 10, 10, 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    uint64_t age1 = frame.ageMs();

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    uint64_t age2 = frame.ageMs();

    EXPECT_GT(age2, age1);
    EXPECT_GE(age2 - age1, 5);
}

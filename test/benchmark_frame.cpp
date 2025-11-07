#include <gtest/gtest.h>
#include "frame.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>

// ============================================================================
// Benchmark Utility (prevent compiler optimization)
// ============================================================================

namespace benchmark {
    template<typename T>
    inline void DoNotOptimize(T& value) {
        asm volatile("" : "+r,m"(value) : : "memory");
    }

    template<typename T>
    inline void DoNotOptimize(T const& value) {
        asm volatile("" : : "r,m"(value) : "memory");
    }
}

// ============================================================================
// Performance Benchmark Tests
// ============================================================================

class FramePerformance : public ::testing::Test {
protected:
    static constexpr int ITERATIONS = 1000;
    static constexpr int WARMUP_ITERATIONS = 100;

    template<typename Func>
    double measureMs(Func&& func, int iterations = ITERATIONS) {
        // Warmup
        for (int i = 0; i < WARMUP_ITERATIONS; ++i) {
            func();
        }

        // Actual measurement
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration<double, std::milli>(end - start).count();
    }

    void printBenchmark(const std::string& name, double total_ms, int iterations = ITERATIONS) {
        double avg_us = (total_ms * 1000.0) / iterations;
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "[BENCHMARK] " << std::setw(40) << std::left << name
                  << " Total: " << std::setw(8) << total_ms << " ms"
                  << " | Avg: " << std::setw(8) << avg_us << " µs/op"
                  << " | Ops/sec: " << std::setw(10) << (iterations / (total_ms / 1000.0))
                  << std::endl;
    }
};

// ============================================================================
// Construction Benchmarks
// ============================================================================

TEST_F(FramePerformance, DefaultConstruction) {
    double time_ms = measureMs([]() {
        Frame frame;
        benchmark::DoNotOptimize(frame);
    });

    printBenchmark("Default construction", time_ms);
    EXPECT_LT(time_ms / ITERATIONS, 0.001);  // Should be < 1 microsecond per op
}

TEST_F(FramePerformance, VGAFrameConstruction) {
    const size_t vga_size = 640 * 480 * 3;

    double time_ms = measureMs([vga_size]() {
        std::vector<uint8_t> data(vga_size);
        Frame frame(std::move(data), 640, 480, 3);
        benchmark::DoNotOptimize(frame);
    });

    printBenchmark("VGA frame (640x480x3) construction", time_ms);
}

TEST_F(FramePerformance, HDFrameConstruction) {
    const size_t hd_size = 1920 * 1080 * 3;

    double time_ms = measureMs([hd_size]() {
        std::vector<uint8_t> data(hd_size);
        Frame frame(std::move(data), 1920, 1080, 3);
        benchmark::DoNotOptimize(frame);
    });

    printBenchmark("HD frame (1920x1080x3) construction", time_ms);
}

TEST_F(FramePerformance, UHDFrameConstruction) {
    const size_t uhd_size = 3840 * 2160 * 3;
    const int iterations = 100;  // Fewer iterations for large frames

    double time_ms = measureMs([uhd_size]() {
        std::vector<uint8_t> data(uhd_size);
        Frame frame(std::move(data), 3840, 2160, 3);
        benchmark::DoNotOptimize(frame);
    }, iterations);

    printBenchmark("4K frame (3840x2160x3) construction", time_ms, iterations);
}

// ============================================================================
// Copy vs Move Benchmarks
// ============================================================================

TEST_F(FramePerformance, CopyConstructorVGA) {
    const size_t vga_size = 640 * 480 * 3;
    Frame source(std::vector<uint8_t>(vga_size), 640, 480, 3);

    double time_ms = measureMs([&source]() {
        Frame copy(source);
        benchmark::DoNotOptimize(copy);
    });

    printBenchmark("VGA frame copy construction", time_ms);
}

TEST_F(FramePerformance, MoveConstructorVGA) {
    const size_t vga_size = 640 * 480 * 3;

    double time_ms = measureMs([vga_size]() {
        Frame source(std::vector<uint8_t>(vga_size), 640, 480, 3);
        Frame moved(std::move(source));
        benchmark::DoNotOptimize(moved);
    });

    printBenchmark("VGA frame move construction", time_ms);
    std::cout << "    → Move should be ~100-1000x faster than copy" << std::endl;
}

TEST_F(FramePerformance, CopyVsMoveSpeedup) {
    const size_t hd_size = 1920 * 1080 * 3;

    // Measure copy
    Frame source(std::vector<uint8_t>(hd_size), 1920, 1080, 3);
    double copy_ms = measureMs([&source]() {
        Frame copy(source);
        benchmark::DoNotOptimize(copy);
    });

    // Measure move
    double move_ms = measureMs([hd_size]() {
        Frame source(std::vector<uint8_t>(hd_size), 1920, 1080, 3);
        Frame moved(std::move(source));
        benchmark::DoNotOptimize(moved);
    });

    printBenchmark("HD frame COPY construction", copy_ms);
    printBenchmark("HD frame MOVE construction", move_ms);

    double speedup = copy_ms / move_ms;
    std::cout << "    → Move is " << std::fixed << std::setprecision(1)
              << speedup << "x faster than copy" << std::endl;

    EXPECT_GT(speedup, 10.0);  // Move should be at least 10x faster
}

// ============================================================================
// Assignment Benchmarks
// ============================================================================

TEST_F(FramePerformance, CopyAssignment) {
    const size_t vga_size = 640 * 480 * 3;
    Frame source(std::vector<uint8_t>(vga_size), 640, 480, 3);
    Frame dest;

    double time_ms = measureMs([&source, &dest]() {
        dest = source;
        benchmark::DoNotOptimize(dest);
    });

    printBenchmark("VGA frame copy assignment", time_ms);
}

TEST_F(FramePerformance, MoveAssignment) {
    const size_t vga_size = 640 * 480 * 3;
    Frame dest;

    double time_ms = measureMs([vga_size, &dest]() {
        Frame source(std::vector<uint8_t>(vga_size), 640, 480, 3);
        dest = std::move(source);
        benchmark::DoNotOptimize(dest);
    });

    printBenchmark("VGA frame move assignment", time_ms);
}

// ============================================================================
// Clone Benchmark
// ============================================================================

TEST_F(FramePerformance, CloneVGA) {
    const size_t vga_size = 640 * 480 * 3;
    Frame source(std::vector<uint8_t>(vga_size), 640, 480, 3);

    double time_ms = measureMs([&source]() {
        Frame cloned = source.clone();
        benchmark::DoNotOptimize(cloned);
    });

    printBenchmark("VGA frame clone", time_ms);
}

// ============================================================================
// Swap Benchmark
// ============================================================================

TEST_F(FramePerformance, SwapFrames) {
    const size_t vga_size = 640 * 480 * 3;
    Frame frame1(std::vector<uint8_t>(vga_size), 640, 480, 3);
    Frame frame2(std::vector<uint8_t>(vga_size / 2), 320, 240, 3);

    double time_ms = measureMs([&frame1, &frame2]() {
        frame1.swap(frame2);
        benchmark::DoNotOptimize(frame1);
        benchmark::DoNotOptimize(frame2);
    });

    printBenchmark("Swap frames", time_ms);
    std::cout << "    → Swap should be constant time (pointer swaps)" << std::endl;
}

// ============================================================================
// Accessor Benchmarks
// ============================================================================

TEST_F(FramePerformance, AccessorCalls) {
    Frame frame(std::vector<uint8_t>(640 * 480 * 3), 640, 480, 3);

    double time_ms = measureMs([&frame]() {
        auto w = frame.width();
        auto h = frame.height();
        auto c = frame.channels();
        auto s = frame.size();
        benchmark::DoNotOptimize(w);
        benchmark::DoNotOptimize(h);
        benchmark::DoNotOptimize(c);
        benchmark::DoNotOptimize(s);
    });

    printBenchmark("Accessor calls (4 getters)", time_ms);
}

// ============================================================================
// Timestamp Benchmarks
// ============================================================================

TEST_F(FramePerformance, TimestampOperations) {
    Frame frame(std::vector<uint8_t>(100), 10, 10, 1);

    double time_ms = measureMs([&frame]() {
        frame.setTimestampNow();
        auto age = frame.ageMs();
        benchmark::DoNotOptimize(age);
    });

    printBenchmark("Timestamp set + age calculation", time_ms);
}

// ============================================================================
// Validation Benchmark
// ============================================================================

TEST_F(FramePerformance, Validation) {
    Frame frame(std::vector<uint8_t>(640 * 480 * 3), 640, 480, 3);

    double time_ms = measureMs([&frame]() {
        bool valid = frame.isValid();
        benchmark::DoNotOptimize(valid);
    });

    printBenchmark("Frame validation check", time_ms);
}

// ============================================================================
// Realistic Workflow Benchmarks
// ============================================================================

TEST_F(FramePerformance, TypicalCaptureWorkflow) {
    const uint32_t width = 640;
    const uint32_t height = 480;
    const uint32_t channels = 3;
    const size_t expected_size = width * height * channels;

    double time_ms = measureMs([width, height, channels, expected_size]() {
        // 1. Create empty frame
        Frame frame;

        // 2. Set dimensions and reserve
        frame.setDimensions(width, height, channels);
        frame.reserve(expected_size);

        // 3. Simulate capture
        std::vector<uint8_t> data(expected_size, 128);
        frame.setData(std::move(data));

        // 4. Validate
        bool valid = frame.isValid();

        // 5. Move to next stage (encoder)
        Frame encoder_frame = std::move(frame);

        benchmark::DoNotOptimize(encoder_frame);
        benchmark::DoNotOptimize(valid);
    });

    printBenchmark("Complete capture workflow (VGA)", time_ms);
}

TEST_F(FramePerformance, PipelineThroughput) {
    const size_t vga_size = 640 * 480 * 3;
    const int frame_count = 100;

    std::cout << "\n[THROUGHPUT TEST] Processing " << frame_count << " VGA frames..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < frame_count; ++i) {
        // Capture
        std::vector<uint8_t> data(vga_size, static_cast<uint8_t>(i % 256));
        Frame capture_frame(std::move(data), 640, 480, 3);

        // Move to encoder
        Frame encoder_frame = std::move(capture_frame);

        // Validate
        ASSERT_TRUE(encoder_frame.isValid());

        // Move to sender
        Frame sender_frame = std::move(encoder_frame);

        benchmark::DoNotOptimize(sender_frame);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double total_ms = std::chrono::duration<double, std::milli>(end - start).count();

    double fps = frame_count / (total_ms / 1000.0);
    double frame_time_ms = total_ms / frame_count;

    std::cout << "    Total time:  " << std::fixed << std::setprecision(2)
              << total_ms << " ms" << std::endl;
    std::cout << "    Throughput:  " << std::fixed << std::setprecision(1)
              << fps << " FPS" << std::endl;
    std::cout << "    Per-frame:   " << std::fixed << std::setprecision(3)
              << frame_time_ms << " ms/frame" << std::endl;

    // Should easily handle 60+ FPS with VGA frames
    EXPECT_GT(fps, 60.0);
}

// ============================================================================
// Memory Benchmark
// ============================================================================

TEST_F(FramePerformance, MemoryFootprint) {
    const size_t hd_size = 1920 * 1080 * 3;

    std::cout << "\n[MEMORY] Frame memory footprint analysis:" << std::endl;

    // Empty frame
    Frame empty;
    std::cout << "    Empty frame metadata:     ~" << sizeof(Frame) << " bytes" << std::endl;

    // HD frame
    Frame hd_frame(std::vector<uint8_t>(hd_size), 1920, 1080, 3);
    std::cout << "    HD frame data:            " << hd_size << " bytes ("
              << std::fixed << std::setprecision(2) << (hd_size / 1024.0 / 1024.0) << " MB)" << std::endl;
    std::cout << "    HD frame total (approx):  " << (sizeof(Frame) + hd_size) << " bytes" << std::endl;

    // Verify move doesn't duplicate
    const uint8_t* original_ptr = hd_frame.dataPtr();
    Frame moved_frame = std::move(hd_frame);
    EXPECT_EQ(moved_frame.dataPtr(), original_ptr);
    std::cout << "    ✓ Move verified: no data duplication" << std::endl;
}

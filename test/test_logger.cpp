#include <gtest/gtest.h>
#include "logger.hpp"

// Simple smoke tests for Logger
TEST(LoggerTest, BasicUsage) {
    Logger::info("Test info message");
    Logger::warn("Test warn message");
    Logger::error("Test error message");
    // Should not crash
}

TEST(LoggerTest, FormattedMessages) {
    Logger::info("Value: {}", 42);
    Logger::info("Two values: {} and {}", 1, 2);
    // Should not crash
}

TEST(LoggerTest, GetLogger) {
    auto logger = Logger::get();
    EXPECT_TRUE(logger != nullptr);
}

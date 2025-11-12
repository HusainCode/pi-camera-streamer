#include "logger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <vector>

// Initialize static members
std::shared_ptr<spdlog::logger> Logger::s_logger = nullptr;
std::mutex Logger::s_mutex;

void Logger::init(const std::string& filename, const std::string& level)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    if (s_logger) {
        return; // Already initialized
    }

    try {
        // Create console sink with color
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);

        // Create rotating file sink (5MB per file, 3 files max)
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            filename, 5 * 1024 * 1024, 3);
        file_sink->set_level(spdlog::level::trace);

        // Combine sinks
        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        s_logger = std::make_shared<spdlog::logger>("pi-camera", sinks.begin(), sinks.end());

        // Set pattern: [timestamp] [level] message
        s_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

        // Set log level
        setLevel(level);

        // Register with spdlog
        spdlog::register_logger(s_logger);

    } catch (const spdlog::spdlog_ex& ex) {
        // Fallback to console-only if file creation fails
        s_logger = spdlog::stdout_color_mt("pi-camera");
        s_logger->warn("Logger init failed: {}. Using console only.", ex.what());
    }
}

std::shared_ptr<spdlog::logger> Logger::get()
{
    std::lock_guard<std::mutex> lock(s_mutex);

    if (!s_logger) {
        // Auto-initialize with defaults if not yet initialized
        init();
    }

    return s_logger;
}

void Logger::setLevel(const std::string& level)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    if (!s_logger) {
        return;
    }

    if (level == "trace") {
        s_logger->set_level(spdlog::level::trace);
    } else if (level == "debug") {
        s_logger->set_level(spdlog::level::debug);
    } else if (level == "info") {
        s_logger->set_level(spdlog::level::info);
    } else if (level == "warn") {
        s_logger->set_level(spdlog::level::warn);
    } else if (level == "error") {
        s_logger->set_level(spdlog::level::err);
    } else if (level == "critical") {
        s_logger->set_level(spdlog::level::critical);
    } else if (level == "off") {
        s_logger->set_level(spdlog::level::off);
    } else {
        s_logger->set_level(spdlog::level::info);
    }
}

void Logger::shutdown()
{
    std::lock_guard<std::mutex> lock(s_mutex);

    if (s_logger) {
        s_logger->flush();
        spdlog::drop("pi-camera");
        s_logger.reset();
    }
}

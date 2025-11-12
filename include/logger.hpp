#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

/**
 * @brief Simple logging wrapper around spdlog
 *
 * Provides thread-safe logging with console and file output.
 * Singleton pattern for global access.
 */
class Logger
{
public:
    /**
     * @brief Initialize the logger with console and file sinks
     * @param filename Log file name (default: "app.log")
     * @param level Logging level (default: "info")
     */
    static void init(const std::string& filename = "app.log", const std::string& level = "info");

    /**
     * @brief Get the singleton logger instance
     */
    static std::shared_ptr<spdlog::logger> get();

    /**
     * @brief Set log level dynamically
     * @param level One of: "trace", "debug", "info", "warn", "error", "critical", "off"
     */
    static void setLevel(const std::string& level);

    /**
     * @brief Shutdown logger and flush all pending messages
     */
    static void shutdown();

    // Convenience logging methods
    template<typename... Args>
    static void trace(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        get()->trace(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        get()->debug(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        get()->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void warn(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        get()->warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        get()->error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void critical(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        get()->critical(fmt, std::forward<Args>(args)...);
    }

private:
    static std::shared_ptr<spdlog::logger> s_logger;
    static std::mutex s_mutex;
};

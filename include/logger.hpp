#pragma

#include <iostream> // (optional) raw fallback logging with std::cout
#include <fstream>  // (optional) for manual file output if spdlog is unavailable
#include <mutex>    // (optional) thread-safety for manual fallback loggers
#include <string>   // std::string for log messages
#include <memory>   // smart pointers for logger instances
#include <chrono>   // timestamps + durations
#include <ctime>    // format system time into human-readable strings

// spdlog headers (the real logging engine)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ostream_sink.h>

namespace logging
{

    // ----------------------------------------------------------
    // Logger (singleton-style, global access)
    // ----------------------------------------------------------
    class Logger
    {
    public:
        static void init(const std::string &filename = "app.log")
        {
            if (logger_)
            {
                return; // prevent double init
            }

            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                filename, 5 * 1024 * 1024, 3);

            // Combine sinks into one logger
            std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        };
    };

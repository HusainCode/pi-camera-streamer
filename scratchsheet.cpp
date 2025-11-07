// #pragma once

// #include <iostream> // (optional) raw fallback logging with std::cout
// #include <fstream>  // (optional) for manual file output if spdlog is unavailable
// #include <mutex>    // (optional) thread-safety for manual fallback loggers
// #include <string>   // std::string for log messages
// #include <memory>   // smart pointers for logger instances
// #include <chrono>   // timestamps + durations
// #include <ctime>    // format system time into human-readable strings

// // spdlog headers (the real logging engine)
// #include <spdlog/spdlog.h>
// #include <spdlog/sinks/stdout_color_sinks.h>
// #include <spdlog/sinks/rotating_file_sink.h>

// namespace logging
// {

//     // ----------------------------------------------------------
//     // Logger (singleton-style, global access)
//     // ----------------------------------------------------------
//     class Logger
//     {
//     public:
//         // Initialize once (e.g., at program startup in main())
//         static void init(const std::string &filename = "app.log")
//         {
//             if (logger_)
//                 return; // prevent double init

//             auto console_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();

//             // Sink 2: Rotating file (max 5 MB, keep 3 backups)
//             auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
//                 filename, 5 * 1024 * 1024, 3);

//             // Combine sinks into one logger
//             std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
//             logger_ = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());

//             // Set global logger
//             spdlog::set_default_logger(logger_);

//             // Log format: [YYYY-MM-DD HH:MM:SS.mmm] [LEVEL] [Thread] message
//             spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [T%t] %v");

//             // Default level = debug (adjust as needed: info, warn, error)
//             spdlog::set_level(spdlog::level::debug);

//             spdlog::info("Logger initialized. Output -> console + {}", filename);
//         }

//         // Get logger instance (to log manually)
//         static std::shared_ptr<spdlog::logger> &get()
//         {
//             return logger_;
//         }

//     private:
//         static std::shared_ptr<spdlog::logger> logger_;
//     };

//     // Definition of the static member
//     inline std::shared_ptr<spdlog::logger> Logger::logger_ = nullptr;

//     // ----------------------------------------------------------
//     // ScopeLogger (RAII: logs entry + exit automatically)
//     // ----------------------------------------------------------
//     class ScopeLogger
//     {
//     public:
//         explicit ScopeLogger(const std::string &name)
//             : name_(name), start_(std::chrono::steady_clock::now())
//         {
//             spdlog::info("[ENTER] {}", name_);
//         }

//         ~ScopeLogger()
//         {
//             auto end = std::chrono::steady_clock::now();
//             auto duration =
//                 std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
//             spdlog::info("[EXIT] {} ({} ms)", name_, duration);
//         }

//     private:
//         std::string name_;
//         std::chrono::time_point<std::chrono::steady_clock> start_;
//     };

// } // namespace logging

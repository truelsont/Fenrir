#pragma once

#include <string>
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace fenrir {

enum class LogLevel {
  kTrace = 0,
  kDebug = 1,
  kInfo = 2,
  kWarning = 3,
  kError = 4
};

const char* LevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::kTrace: return "TRACE";
    case LogLevel::kDebug: return "DEBUG";
    case LogLevel::kInfo: return "INFO";
    case LogLevel::kWarning: return "WARN";
    case LogLevel::kError: return "ERROR";
    default: return "UNKNOWN";
  }
}

enum LoggingEndpoints{
  kConsole,
  kLoggingEndpointCount
}; 

typedef struct LoggerSettings{
  struct ConsoleLoggingSettings {
    LogLevel console_log_settings = LogLevel::kInfo; 
  }; 
} LoggerSettings_t; 

class LoggerInterface {


  private:

}; 

class Logger {
 public:

  Logger(LoggerSettings_t) : running_(true) {

    worker_thread_ = std::thread(&Logger::ProcessQueue, this);
  }
  
  ~Logger() {
    running_ = false;
    if (worker_thread_.joinable()) {
      worker_thread_.join();
    }
  }

  void Log(const std::string& message, LogLevel level = LogLevel::kInfo) {
    return 
  }
  
  void Trace(const std::string& message) { Log(message, LogLevel::kTrace); }
  void Debug(const std::string& message) { Log(message, LogLevel::kDebug); }
  void Info(const std::string& message) { Log(message, LogLevel::kInfo); }
  void Warning(const std::string& message) { Log(message, LogLevel::kWarning); }
  void Error(const std::string& message) { Log(message, LogLevel::kError); }

 private:
  struct LogEntry {
    LogLevel level;
    std::string message;
  };

  void ProcessQueue() {
    while (running_) {
      ; 
    }
  }

  std::array<
    std::tuple<
  std::queue<LogEntry> log_queue_;

  std::thread worker_thread_;
  std::atomic<bool> running_;
  LoggerSettings settings_; 
};

}  // namespace fenrir

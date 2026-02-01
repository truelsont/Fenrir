#pragma once

#include <string>
#include <iostream>

namespace fenrir {

enum class LogLevel {
  kTrace = 0,
  kDebug = 1,
  kInfo = 2,
  kWarning = 3,
  kError = 4
};

inline const char* LevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::kTrace: return "TRACE";
    case LogLevel::kDebug: return "DEBUG";
    case LogLevel::kInfo: return "INFO";
    case LogLevel::kWarning: return "WARN";
    case LogLevel::kError: return "ERROR";
    default: return "UNKNOWN";
  }
}

// Simple synchronous logger stub
// TODO: Replace with async worker thread implementation
class Logger {
 public:
  Logger() : min_level_(LogLevel::kInfo) {}

  void SetLevel(LogLevel level) { min_level_ = level; }

  void Log(const std::string& message, LogLevel level = LogLevel::kInfo) {
    if (level >= min_level_) {
      std::cout << "[" << LevelToString(level) << "] " << message << std::endl;
    }
  }

  void Trace(const std::string& message) { Log(message, LogLevel::kTrace); }
  void Debug(const std::string& message) { Log(message, LogLevel::kDebug); }
  void Info(const std::string& message) { Log(message, LogLevel::kInfo); }
  void Warning(const std::string& message) { Log(message, LogLevel::kWarning); }
  void Error(const std::string& message) { Log(message, LogLevel::kError); }

 private:
  LogLevel min_level_;
};

}  // namespace fenrir

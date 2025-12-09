#ifndef FENRIR_SRC_UTIL_LOGGER_LOGGER_HH_
#define FENRIR_SRC_UTIL_LOGGER_LOGGER_HH_

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

class Logger {
 public:
  Logger() : running_(true) {
    worker_thread_ = std::thread(&Logger::ProcessQueue, this);
  }
  
  ~Logger() {
    running_ = false;
    cv_.notify_one();
    if (worker_thread_.joinable()) {
      worker_thread_.join();
    }
  }

  void Log(const std::string& message, LogLevel level = LogLevel::kInfo) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    log_queue_.push({level, message});
    cv_.notify_one();
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
      std::unique_lock<std::mutex> lock(queue_mutex_);
      cv_.wait(lock, [this] { return !log_queue_.empty() || !running_; });
      
      while (!log_queue_.empty()) {
        LogEntry entry = log_queue_.front();
        log_queue_.pop();
        lock.unlock();
        
        // Write to stdout (non-blocking from main thread perspective)
        std::cout << "[" << LevelToString(entry.level) << "] " 
                  << entry.message << std::endl;
        
        lock.lock();
      }
    }
  }
  
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

  std::queue<LogEntry> log_queue_;
  std::mutex queue_mutex_;
  std::condition_variable cv_;
  std::thread worker_thread_;
  std::atomic<bool> running_;
};

}  // namespace fenrir

#endif  // FENRIR_SRC_UTIL_LOGGER_LOGGER_HH_

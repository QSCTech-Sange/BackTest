#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/spdlog.h"

// 简单封装了一个 logger 单例
class Logger {
public:
  static Logger &instance() {
    static Logger logger;
    return logger;
  }

  spdlog::logger &get() { return *logger_; }

  #define LOG(level, ...) Logger::instance().get().level(__VA_ARGS__)
  
private:
  Logger() {
    const char *logPathEnv = std::getenv("BACKTEST_LOG");
    std::string logPath = logPathEnv ? std::string(logPathEnv)+"/BackTest" : "logs/BackTest";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(
        logPath, 23, 59));
    logger_ =
        std::make_shared<spdlog::logger>("BackTest", begin(sinks), end(sinks));
    logger_->info("Log Path {}", logPath);
  }

  std::shared_ptr<spdlog::logger> logger_;
};
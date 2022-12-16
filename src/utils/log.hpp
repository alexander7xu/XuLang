#ifndef _SRC_UTILS_LOG_HPP
#define _SRC_UTILS_LOG_HPP

#include <initializer_list>
#include <iostream>
#include <unordered_map>

#include "./utils.hpp"

namespace utils {

class Logger final {
 private:
  class LogFunc final {
    const int _level;
    const char *const _prefix;
    const int *const _logger_level;

   public:
    LogFunc(int level, const char *const prefix, const int *logger_level)
        : _level(level), _prefix(prefix), _logger_level(logger_level){};
    void operator()(const std::initializer_list<std::string> &list,
                    const std::string &inter = " ",
                    const std::string &end = "\n") const {
      if (_level < *_logger_level) return;
      std::cerr << _prefix;
      for (const auto &item : list) std::cerr << item << inter;
      std::cerr << end << "\e[m";
    }
  };

 private:
  std::string _name;
  int _level;
  Logger(const std::string &name, int level = kLevelDefault)
      : _name(name), _level(level) {}
  Logger(const Logger &) = delete;

 public:
  void SetLevel(int level) { _level = level; }

  const LogFunc Debug =  // Blue
      LogFunc(kLevelDebug, "\e[94;1m[DEBUG]\e[0;94m ", &_level);
  const LogFunc Info =  // White
      LogFunc(kLevelInfo, "\e[99;1m[INFO]\e[0;99m ", &_level);
  const LogFunc Warning =  // Yellow
      LogFunc(kLevelWarning, "\e[93;1m[WARNING]\e[0;93m ", &_level);
  const LogFunc Error =  // Red
      LogFunc(kLevelError, "\e[91;1m[ERROR]\e[0;91m ", &_level);
  const LogFunc Critical =  // Magenta
      LogFunc(kLevelCritical, "\e[95;1m[CRITICAL]\e[0;95m ", &_level);

 public:
  inline static const int kLevelDefault = 0;
  inline static const int kLevelDebug = 10;
  inline static const int kLevelInfo = 20;
  inline static const int kLevelWarning = 30;
  inline static const int kLevelError = 40;
  inline static const int kLevelCritical = 50;

  static std::shared_ptr<Logger> NewLogger(const std::string &name,
                                           int level = kLevelDefault) {
    if (kLoggerMap.find(name) != kLoggerMap.end()) return nullptr;
    auto res = kLoggerMap[name] =
        std::shared_ptr<Logger>(new Logger(name, kLevelDefault));
    return res;
  }

  static std::shared_ptr<Logger> GetLogger(const std::string &name) {
    auto it = kLoggerMap.find(name);
    return it != kLoggerMap.end() ? it->second : nullptr;
  }

 private:
  inline static std::unordered_map<std::string, std::shared_ptr<Logger>>
      kLoggerMap;
};

}  // namespace utils

#endif  // _SRC_UTILS_LOG_HPP

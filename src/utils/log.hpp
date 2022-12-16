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
    std::string _prefix;
    const int *const _logger_level;
    const int _level;

   public:
    LogFunc(int level, const int *logger_level, const std::string &name,
            const int &color_code)
        : _logger_level(logger_level), _level(level) {
      auto cc = std::to_string(color_code);
      _prefix = std::string("\e[") + cc + ";1m[" + name + "]\e[0;" + cc + "m ";
    };

    inline void operator()(const std::initializer_list<std::string> &list,
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
  inline void SetLevel(int level) { _level = level; }

  const LogFunc Debug =  // Blue
      LogFunc(kLevelDebug, &_level, _name + ".DEBUG", 94);
  const LogFunc Info =  // White
      LogFunc(kLevelDebug, &_level, _name + ".INFO", 99);
  const LogFunc Warning =  // Yellow
      LogFunc(kLevelDebug, &_level, _name + ".WARNING", 93);
  const LogFunc Error =  // Red
      LogFunc(kLevelDebug, &_level, _name + ".ERROR", 91);
  const LogFunc Critical =  // Magenta
      LogFunc(kLevelDebug, &_level, _name + ".CRITICAL", 95);

 public:
  inline static constexpr int kLevelDefault = 0;
  inline static constexpr int kLevelDebug = 10;
  inline static constexpr int kLevelInfo = 20;
  inline static constexpr int kLevelWarning = 30;
  inline static constexpr int kLevelError = 40;
  inline static constexpr int kLevelCritical = 50;

  static std::shared_ptr<Logger> GetLogger(const std::string &name);
  static std::shared_ptr<Logger> NewLogger(const std::string &name,
                                           int level = kLevelDefault);

 private:
  static std::unordered_map<std::string, std::shared_ptr<Logger>> kLoggerMap;
};

}  // namespace utils

#endif  // _SRC_UTILS_LOG_HPP

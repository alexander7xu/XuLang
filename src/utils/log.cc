#include "./log.hpp"

namespace utils {

std::unordered_map<std::string, std::shared_ptr<Logger>> Logger::kLoggerMap;

std::shared_ptr<Logger> Logger::NewLogger(const std::string &name, int level) {
  if (kLoggerMap.find(name) != kLoggerMap.end()) return nullptr;
  auto res = kLoggerMap[name] =
      std::shared_ptr<Logger>(new Logger(name, kLevelDefault));
  return res;
}

std::shared_ptr<Logger> Logger::GetLogger(const std::string &name) {
  auto it = kLoggerMap.find(name);
  return it != kLoggerMap.end() ? it->second : nullptr;
}

}  // namespace utils

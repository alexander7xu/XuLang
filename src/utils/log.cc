#include "./log.hpp"

namespace utils {

std::unordered_map<std::string, std::shared_ptr<Logger>> Logger::kLoggerMap;

utils::Sptr<Logger> Logger::New(const std::string &name, int level) {
  if (kLoggerMap.find(name) != kLoggerMap.end()) return nullptr;
  auto res = kLoggerMap[name] =
      std::shared_ptr<Logger>(new Logger(name, kLevelDefault));
  return res;
}

utils::Sptr<Logger> Logger::Get(const std::string &name, bool auto_new) {
  auto it = kLoggerMap.find(name);
  if (it != kLoggerMap.end()) return it->second;
  return auto_new ? New(name) : nullptr;
}

}  // namespace utils

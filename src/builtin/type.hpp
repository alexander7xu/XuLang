#ifndef _XULANG_SRC_BUILTIN_TYPE_HPP
#define _XULANG_SRC_BUILTIN_TYPE_HPP

#include <cinttypes>
#include <string>

namespace builtin {

class BasicType {
 public:
  virtual ~BasicType() = default;
  virtual const char *const GetName() const = 0;
};

class Int final : public BasicType {
 public:
  virtual const char *const GetName() const override { return "Int"; }
  static constexpr auto BitWidth() { return 64; }
};

class Float final : public BasicType {
 public:
  virtual const char *const GetName() const override { return "Float"; }
  static constexpr auto BitWidth() { return 128; }
};

class String final : public BasicType {
 public:
  virtual const char *const GetName() const override { return "String"; }
};

};  // namespace builtin

#endif  // _XULANG_SRC_BUILTIN_TYPE_HPP

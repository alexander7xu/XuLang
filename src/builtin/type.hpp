#ifndef _XULANG_SRC_BUILTIN_TYPE_HPP
#define _XULANG_SRC_BUILTIN_TYPE_HPP

#include <cinttypes>
#include <string>

namespace builtin {

class BasicType {
 public:
  virtual ~BasicType() = default;
  virtual std::string GetName() const = 0;
};

class Int final : public BasicType {
 public:
  virtual std::string GetName() const override { return "Int"; }
  static constexpr auto BitWidth() { return 64; }
};

class Uint final : public BasicType {
 public:
  virtual std::string GetName() const override { return "Uint"; }
};

class Float final : public BasicType {
 public:
  virtual std::string GetName() const override { return "Float"; }
  static constexpr auto BitWidth() { return 128; }
};

class Byte final : public BasicType {
 public:
  virtual std::string GetName() const override { return "Byte"; }
};

class Pointer final : public BasicType {
 public:
  virtual std::string GetName() const override { return "Pointer"; }
};

class String final : public BasicType {
 public:
  virtual std::string GetName() const override { return "String"; }
};

};  // namespace builtin

#endif  // _XULANG_SRC_BUILTIN_TYPE_HPP

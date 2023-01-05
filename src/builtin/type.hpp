#ifndef _XULANG_SRC_BUILTIN_TYPE_HPP
#define _XULANG_SRC_BUILTIN_TYPE_HPP

#include "../utils/utils.hpp"

namespace builtin {

class BasicType final {
 private:
  const char *const _name;

 public:
  BasicType(const char *name) : _name(name) {}
  const char *GetName() const { return _name; };
};

extern BasicType kBuiltinTypes[3];
extern const BasicType &kInt;
extern const BasicType &kFloat;
extern const BasicType &kString;

};  // namespace builtin

#endif  // _XULANG_SRC_BUILTIN_TYPE_HPP

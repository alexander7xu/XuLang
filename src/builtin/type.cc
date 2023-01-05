#include "./type.hpp"

namespace builtin {

BasicType kBuiltinTypes[3] = {BasicType("Int"), BasicType("Float"),
                              BasicType("String")};

const BasicType &kInt = kBuiltinTypes[0];
const BasicType &kFloat = kBuiltinTypes[1];
const BasicType &kString = kBuiltinTypes[2];

}  // namespace builtin

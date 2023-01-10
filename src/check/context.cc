#include "./context.hpp"

namespace check {

utils::Sptr<utils::Logger> kLog = utils::Logger::New("check");

namespace builtin_type_symbol {
utils::Uptr<TypeSymbol> kBase = TypeSymbol::New("__Base", -1);

utils::Uptr<TypeSymbol> kStruct = TypeSymbol::New("Struct", 2);
utils::Uptr<TypeSymbol> kClass = TypeSymbol::New("Class", 2);
utils::Uptr<TypeSymbol> kFunction = TypeSymbol::New("Function", 2);

utils::Sptr<TypeSymbol> kVoid = TypeSymbol::New("Void", 2);
utils::Sptr<TypeSymbol> kInt = TypeSymbol::New("Int", 2);
utils::Sptr<TypeSymbol> kFloat = TypeSymbol::New("Float", 2);
utils::Sptr<TypeSymbol> kString = TypeSymbol::New("String", 2);

}  // namespace builtin_type_symbol

}  // namespace check

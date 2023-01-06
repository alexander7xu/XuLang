#include "./statement.hpp"

namespace ast {

#define _OVERRIDE_LEAF_ACCEPT(LeafClass) \
  void LeafClass::Accept(VisitorInterface *visitor) { visitor->Visit(this); }

#define _OVERRIDE_LEAF_OPERATOR(LeafOperator, name) \
  _OVERRIDE_LEAF_ACCEPT(LeafOperator)               \
  const char *LeafOperator::GetName() const { return name; }

#define _OVERRIDE_LEAF_CREATE(LeafCreate) \
  _OVERRIDE_LEAF_ACCEPT(LeafCreate)       \
  const TextType &LeafCreate::GetId() const { return *id; }

_OVERRIDE_LEAF_ACCEPT(Module)
_OVERRIDE_LEAF_ACCEPT(Block)
_OVERRIDE_LEAF_ACCEPT(ExprStatement)
_OVERRIDE_LEAF_ACCEPT(Break)
_OVERRIDE_LEAF_ACCEPT(Continue)
_OVERRIDE_LEAF_ACCEPT(Return)
_OVERRIDE_LEAF_ACCEPT(If)
_OVERRIDE_LEAF_ACCEPT(While)
_OVERRIDE_LEAF_ACCEPT(Raise)
_OVERRIDE_LEAF_ACCEPT(Try)

_OVERRIDE_LEAF_CREATE(ObjCreate)
_OVERRIDE_LEAF_CREATE(Function)
_OVERRIDE_LEAF_CREATE(Assemble)
_OVERRIDE_LEAF_CREATE(Struct)
_OVERRIDE_LEAF_CREATE(Class)
_OVERRIDE_LEAF_CREATE(Import)

_OVERRIDE_LEAF_ACCEPT(Literal)
_OVERRIDE_LEAF_ACCEPT(Name)
_OVERRIDE_LEAF_ACCEPT(UnaryOpExpr)
_OVERRIDE_LEAF_ACCEPT(BinaryOpExpr)
_OVERRIDE_LEAF_ACCEPT(AssignOpExpr)
_OVERRIDE_LEAF_ACCEPT(LogicExpr)
_OVERRIDE_LEAF_ACCEPT(IfElseExpr)
_OVERRIDE_LEAF_ACCEPT(CallExpr)
_OVERRIDE_LEAF_ACCEPT(SubscriptExpr)

_OVERRIDE_LEAF_OPERATOR(CallOperator, "__call__")
_OVERRIDE_LEAF_OPERATOR(SubscriptOperator, "__subscript__")

_OVERRIDE_LEAF_OPERATOR(OpPlus, "__plus__")
_OVERRIDE_LEAF_OPERATOR(OpMinus, "__minus__")
_OVERRIDE_LEAF_OPERATOR(OpMul, "__mul__")
_OVERRIDE_LEAF_OPERATOR(OpDiv, "__div__")
_OVERRIDE_LEAF_OPERATOR(OpMod, "__mod__")
_OVERRIDE_LEAF_OPERATOR(OpBitXor, "__bit_xor__")
_OVERRIDE_LEAF_OPERATOR(OpBitOr, "__bit_or__")
_OVERRIDE_LEAF_OPERATOR(OpBitAnd, "__bit_and__")
_OVERRIDE_LEAF_OPERATOR(OpShiftL, "__shift_left__")
_OVERRIDE_LEAF_OPERATOR(OpShiftR, "__shift_right__")

_OVERRIDE_LEAF_OPERATOR(OpAssign, "__assign__")
_OVERRIDE_LEAF_OPERATOR(OpSelfPlus, "__self_plus__")
_OVERRIDE_LEAF_OPERATOR(OpSelfMinus, "__self_minus__")
_OVERRIDE_LEAF_OPERATOR(OpSelfMul, "__self_mul__")
_OVERRIDE_LEAF_OPERATOR(OpSelfDiv, "__self_div__")
_OVERRIDE_LEAF_OPERATOR(OpSelfMod, "__self_mod__")
_OVERRIDE_LEAF_OPERATOR(OpSelfBitXor, "__self_bit_xor__")
_OVERRIDE_LEAF_OPERATOR(OpSelfBitOr, "__self_bit_or__")
_OVERRIDE_LEAF_OPERATOR(OpSelfBitAnd, "__self_bit_and__")
_OVERRIDE_LEAF_OPERATOR(OpSelfShiftL, "__self_shift_left__")
_OVERRIDE_LEAF_OPERATOR(OpSelfShiftR, "__self_shift_right__")

_OVERRIDE_LEAF_OPERATOR(OpOr, "__or__")
_OVERRIDE_LEAF_OPERATOR(OpAnd, "__and__")
_OVERRIDE_LEAF_OPERATOR(OpEq, "__eq__")
_OVERRIDE_LEAF_OPERATOR(OpNe, "__ne__")
_OVERRIDE_LEAF_OPERATOR(OpLe, "__le__")
_OVERRIDE_LEAF_OPERATOR(OpGe, "__ge__")
_OVERRIDE_LEAF_OPERATOR(OpLt, "__lt__")
_OVERRIDE_LEAF_OPERATOR(OpGt, "__gt__")

_OVERRIDE_LEAF_OPERATOR(OpBitNot, "__bit_not__")
_OVERRIDE_LEAF_OPERATOR(OpNot, "__not__")
_OVERRIDE_LEAF_OPERATOR(OpPositive, "__positive__")
_OVERRIDE_LEAF_OPERATOR(OpNegative, "__negative__")
_OVERRIDE_LEAF_OPERATOR(OpDeref, "__deref__")
_OVERRIDE_LEAF_OPERATOR(OpRef, "__ref__")

}  // namespace ast

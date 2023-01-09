#include "./statement.hpp"

namespace ast {

void VisitorInterface::Visit(class OpPlus *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpMinus *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpMul *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpDiv *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpMod *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpBitXor *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpBitOr *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpBitAnd *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpShiftL *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpShiftR *) { throw "Unimplemented"; }

void VisitorInterface::Visit(class OpAssign *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfPlus *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfMinus *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfMul *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfDiv *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfMod *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfBitXor *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfBitOr *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfBitAnd *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfShiftL *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpSelfShiftR *) { throw "Unimplemented"; }

void VisitorInterface::Visit(class OpOr *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpAnd *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpEq *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpNe *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpLe *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpGe *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpLt *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpGt *) { throw "Unimplemented"; }

void VisitorInterface::Visit(class OpBitNot *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpNot *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpPositive *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpNegative *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpDeref *) { throw "Unimplemented"; }
void VisitorInterface::Visit(class OpRef *) { throw "Unimplemented"; }

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

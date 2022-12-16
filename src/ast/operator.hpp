#ifndef _XULANG_SRC_AST_OPERATOR_HPP
#define _XULANG_SRC_AST_OPERATOR_HPP

#include <iostream>

#include "./node.hpp"

namespace ast {

using utils::Uptr;

class Expression;

class Operator : public Node {
 public:
  virtual std::string GetName() const = 0;
};

class UnaryOperator : public Operator {};
class BinaryOperator : public Operator {};
class LogicOperator : public Operator {};

class CallOperator final : public Operator {
 public:
  std::list<Uptr<Expression>> unameds;
  std::list<std::tuple<Uptr<TextType>, Uptr<Expression>>> keywords;

  CallOperator() = default;
  virtual std::string GetName() const override { return "__call__"; };
  virtual void Accept(VisitorInterface *visitor) override {
    visitor->Visit(this);
  }

  void AddUnamed(Uptr<Expression> &&unamed) {
    unameds.push_back(std::move(unamed));
  }
  void AddKeyword(Uptr<TextType> &&name, Uptr<Expression> &&val) {
    keywords.push_back(std::make_tuple(std::move(name), std::move(val)));
  }
};

class SubscriptOperator final : public Operator {
 public:
  SubscriptOperator() = default;
  // e.g. [beg:end:step], [beg:end], [beg], [:end], [::step], [beg::step]
  using SubscriptArg =
      std::tuple<Uptr<Expression>, Uptr<Expression>, Uptr<Expression>>;
  std::list<Uptr<SubscriptArg>> dims;
  virtual std::string GetName() const override { return "__subscript__"; };
  virtual void Accept(VisitorInterface *visitor) override {
    visitor->Visit(this);
  }

  void AddDim(Uptr<SubscriptArg> &&dim) {
    dims.push_back(std::move(dim));
  }
};

#define _OP_CHILD_CLASS(class_name, inbuilt_name, parent)                 \
  class_name final : public parent {                                      \
   public:                                                                \
    class_name() = default;                                               \
    virtual std::string GetName() const override { return inbuilt_name; } \
    virtual void Accept(VisitorInterface *visitor) override {             \
      visitor->Visit(this);                                               \
    }                                                                     \
  }

#define _BOP_CHILD_CLASS(class_name, inbuilt_name) \
  _OP_CHILD_CLASS(class_name, inbuilt_name, BinaryOperator)

#define _UOP_CHILD_CLASS(class_name, inbuilt_name) \
  _OP_CHILD_CLASS(class_name, inbuilt_name, UnaryOperator)

#define _LOP_CHILD_CLASS(class_name, inbuilt_name) \
  _OP_CHILD_CLASS(class_name, inbuilt_name, LogicOperator)

class _BOP_CHILD_CLASS(OpPlus, "__plus__");
class _BOP_CHILD_CLASS(OpMinus, "__minus__");
class _BOP_CHILD_CLASS(OpMul, "__mul__");
class _BOP_CHILD_CLASS(OpDiv, "__div__");
class _BOP_CHILD_CLASS(OpMod, "__mod__");
class _BOP_CHILD_CLASS(OpBitXor, "__bit_xor__");
class _BOP_CHILD_CLASS(OpBitOr, "__bit_or__");
class _BOP_CHILD_CLASS(OpBitAnd, "__bit_and__");
class _BOP_CHILD_CLASS(OpShiftL, "__shift_left__");
class _BOP_CHILD_CLASS(OpShiftR, "__shift_right__");

class _BOP_CHILD_CLASS(OpAssign, "__assign__");
class _BOP_CHILD_CLASS(OpSelfPlus, "__self_plus__");
class _BOP_CHILD_CLASS(OpSelfMinus, "__self_minus__");
class _BOP_CHILD_CLASS(OpSelfMul, "__self_mul__");
class _BOP_CHILD_CLASS(OpSelfDiv, "__self_div__");
class _BOP_CHILD_CLASS(OpSelfMod, "__self_mod__");
class _BOP_CHILD_CLASS(OpSelfBitXor, "__self_bit_xor__");
class _BOP_CHILD_CLASS(OpSelfBitOr, "__self_bit_or__");
class _BOP_CHILD_CLASS(OpSelfBitAnd, "__self_bit_and__");
class _BOP_CHILD_CLASS(OpSelfShiftL, "__self_shift_left__");
class _BOP_CHILD_CLASS(OpSelfShiftR, "__self_shift_right__");

class _LOP_CHILD_CLASS(OpOr, "__or__");
class _LOP_CHILD_CLASS(OpAnd, "__and__");
class _LOP_CHILD_CLASS(OpEq, "__eq__");
class _LOP_CHILD_CLASS(OpNe, "__ne__");
class _LOP_CHILD_CLASS(OpLe, "__le__");
class _LOP_CHILD_CLASS(OpGe, "__ge__");
class _LOP_CHILD_CLASS(OpLt, "__lt__");
class _LOP_CHILD_CLASS(OpGt, "__gt__");

class _UOP_CHILD_CLASS(OpBitNot, "__bit_not__");
class _UOP_CHILD_CLASS(OpNot, "__not__");
class _UOP_CHILD_CLASS(OpPositive, "__positive__");
class _UOP_CHILD_CLASS(OpNegative, "__negative__");
class _UOP_CHILD_CLASS(OpDeref, "__deref__");
class _UOP_CHILD_CLASS(OpRef, "__ref__");

}  // namespace ast

#endif  // _XULANG_SRC_AST_OPERATOR_HPP

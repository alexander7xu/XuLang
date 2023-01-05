#ifndef _XULANG_SRC_AST_OPERATOR_HPP
#define _XULANG_SRC_AST_OPERATOR_HPP

#include <iostream>

#include "./node.hpp"

namespace ast {

using utils::Uptr;

class Expression;

class Operator : public Node {
 public:
  virtual const char *GetName() const = 0;
};

class UnaryOperator : public Operator {};
class BinaryOperator : public Operator {};
class LogicOperator : public Operator {};

// e.g.
// x := Function(unameds0, u:type_args0, v:type_args1, x:=keywords0) {}
// x(unameds0, v:=keywords0)
class CallOperator final : public Operator {
 public:
  std::list<Uptr<Expression>> unameds;
  std::list<std::tuple<Uptr<TextType>, Uptr<Name>>> type_args;
  std::list<std::tuple<Uptr<TextType>, Uptr<Expression>>> keywords;

  CallOperator() = default;
  virtual const char *GetName() const override;
  virtual void Accept(VisitorInterface *) override;

  inline void AddUnamed(Uptr<Expression> &&unamed) {
    unameds.push_back(std::move(unamed));
  }
  void AddTypeArg(Uptr<TextType> &&id, Uptr<Name> &&type) {
    type_args.push_back({std::move(id), std::move(type)});
  }
  inline void AddKeyword(Uptr<TextType> &&name, Uptr<Expression> &&val) {
    keywords.push_back(std::make_tuple(std::move(name), std::move(val)));
  }
};

class SubscriptOperator final : public Operator {
 public:
  // e.g. [beg:end:step], [beg:end], [beg], [:end], [::step], [beg::step]
  using SubscriptArg =
      std::tuple<Uptr<Expression>, Uptr<Expression>, Uptr<Expression>>;
  std::list<Uptr<SubscriptArg>> dims;
  SubscriptOperator() = default;
  virtual const char *GetName() const override;
  virtual void Accept(VisitorInterface *) override;

  inline void AddDim(Uptr<SubscriptArg> &&dim) {
    dims.push_back(std::move(dim));
  }
};

#define _OP_CHILD_CLASS(class_name, parent)                  \
  class_name final : public parent {                         \
   public:                                                   \
    class_name() = default;                                  \
    virtual const char *GetName() const override;            \
    virtual void Accept(VisitorInterface *visitor) override; \
  }

#define _BOP_CHILD_CLASS(class_name) _OP_CHILD_CLASS(class_name, BinaryOperator)
#define _LOP_CHILD_CLASS(class_name) _OP_CHILD_CLASS(class_name, LogicOperator)
#define _UOP_CHILD_CLASS(class_name) _OP_CHILD_CLASS(class_name, UnaryOperator)

class _BOP_CHILD_CLASS(OpPlus);
class _BOP_CHILD_CLASS(OpMinus);
class _BOP_CHILD_CLASS(OpMul);
class _BOP_CHILD_CLASS(OpDiv);
class _BOP_CHILD_CLASS(OpMod);
class _BOP_CHILD_CLASS(OpBitXor);
class _BOP_CHILD_CLASS(OpBitOr);
class _BOP_CHILD_CLASS(OpBitAnd);
class _BOP_CHILD_CLASS(OpShiftL);
class _BOP_CHILD_CLASS(OpShiftR);

class _BOP_CHILD_CLASS(OpAssign);
class _BOP_CHILD_CLASS(OpSelfPlus);
class _BOP_CHILD_CLASS(OpSelfMinus);
class _BOP_CHILD_CLASS(OpSelfMul);
class _BOP_CHILD_CLASS(OpSelfDiv);
class _BOP_CHILD_CLASS(OpSelfMod);
class _BOP_CHILD_CLASS(OpSelfBitXor);
class _BOP_CHILD_CLASS(OpSelfBitOr);
class _BOP_CHILD_CLASS(OpSelfBitAnd);
class _BOP_CHILD_CLASS(OpSelfShiftL);
class _BOP_CHILD_CLASS(OpSelfShiftR);

class _LOP_CHILD_CLASS(OpOr);
class _LOP_CHILD_CLASS(OpAnd);
class _LOP_CHILD_CLASS(OpEq);
class _LOP_CHILD_CLASS(OpNe);
class _LOP_CHILD_CLASS(OpLe);
class _LOP_CHILD_CLASS(OpGe);
class _LOP_CHILD_CLASS(OpLt);
class _LOP_CHILD_CLASS(OpGt);

class _UOP_CHILD_CLASS(OpBitNot);
class _UOP_CHILD_CLASS(OpNot);
class _UOP_CHILD_CLASS(OpPositive);
class _UOP_CHILD_CLASS(OpNegative);
class _UOP_CHILD_CLASS(OpDeref);
class _UOP_CHILD_CLASS(OpRef);

}  // namespace ast

#endif  // _XULANG_SRC_AST_OPERATOR_HPP

#ifndef _XULANG_SRC_AST_EXPRESSION_HPP
#define _XULANG_SRC_AST_EXPRESSION_HPP

#include "../builtin/type.hpp"
#include "./operator.hpp"

namespace ast {

class Expression : public Node {};

class Literal final : public Expression {
 public:
  Uptr<TextType> val;
  const builtin::BasicType &type;
  Literal(Uptr<TextType> &&val, const builtin::BasicType &basic_type)
      : val(std::move(val)), type(basic_type) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. parent.id (deref=false) or parent->id (deref=true) or id
class Name final : public Expression {
 public:
  Uptr<TextType> id;
  bool deref;
  Uptr<Expression> parent;
  Name(Uptr<TextType> &&id, bool deref = false,
       Uptr<Expression> &&parent = nullptr)
      : id(std::move(id)), deref(deref), parent(std::move(parent)) {}
  virtual void Accept(VisitorInterface *) override;
};

class UnaryOpExpr final : public Expression {
 public:
  Uptr<UnaryOperator> op;
  Uptr<Expression> right;
  UnaryOpExpr(Uptr<UnaryOperator> &&op, Uptr<Expression> &&right)
      : op(std::move(op)), right(std::move(right)) {}
  virtual void Accept(VisitorInterface *) override;
};

class BinaryOpExpr final : public Expression {
 public:
  Uptr<Expression> left;
  Uptr<BinaryOperator> op;
  Uptr<Expression> right;
  BinaryOpExpr(Uptr<Expression> &&left, Uptr<BinaryOperator> &&op,
               Uptr<Expression> &&right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
  virtual void Accept(VisitorInterface *) override;
};

class LogicExpr final : public Expression {
 public:
  Uptr<Expression> left;
  Uptr<LogicOperator> op;
  Uptr<Expression> right;
  LogicExpr(Uptr<Expression> &&left, Uptr<LogicOperator> &&op,
            Uptr<Expression> &&right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. ExpL if Cond else ExpR
class IfElseExpr final : public Expression {
 public:
  Uptr<Expression> left;
  Uptr<Expression> test;
  Uptr<Expression> right;
  IfElseExpr(Uptr<Expression> &&left, Uptr<Expression> &&test,
             Uptr<Expression> &&right)
      : left(std::move(left)), test(std::move(test)), right(std::move(right)) {}
  virtual void Accept(VisitorInterface *) override;
};

class CallExpr final : public Expression {
 public:
  Uptr<Expression> obj;
  Uptr<CallOperator> op;
  CallExpr(Uptr<Expression> &&obj, Uptr<CallOperator> &&op)
      : obj(std::move(obj)), op(std::move(op)) {}
  virtual void Accept(VisitorInterface *) override;
};

class SubscriptExpr final : public Expression {
 public:
  Uptr<Expression> obj;
  Uptr<SubscriptOperator> op;
  SubscriptExpr(Uptr<Expression> &&obj, Uptr<SubscriptOperator> &&op)
      : obj(std::move(obj)), op(std::move(op)) {}
  virtual void Accept(VisitorInterface *) override;
};

}  // namespace ast

#endif  // _XULANG_SRC_AST_EXPRESSION_HPP

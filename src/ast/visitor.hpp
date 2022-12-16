#ifndef _XULANG_SRC_AST_VISITOR_HPP
#define _XULANG_SRC_AST_VISITOR_HPP

#include "../utils/utils.hpp"

namespace ast {

class VisitorInterface {
 public:
  virtual ~VisitorInterface() = default;

  virtual void Visit(class Module *) = 0;
  virtual void Visit(class Block *) = 0;
  virtual void Visit(class ExprStatement *) = 0;
  virtual void Visit(class Break *) = 0;
  virtual void Visit(class Continue *) = 0;
  virtual void Visit(class Return *) = 0;
  virtual void Visit(class If *) = 0;
  virtual void Visit(class While *) = 0;
  virtual void Visit(class ObjCreate *) = 0;
  virtual void Visit(class Function *) = 0;
  virtual void Visit(class Assemble *) = 0;
  virtual void Visit(class Struct *) = 0;
  virtual void Visit(class Class *) = 0;
  virtual void Visit(class Import *) = 0;
  virtual void Visit(class Raise *) = 0;
  virtual void Visit(class Try *) = 0;

  virtual void Visit(class Literal *) = 0;
  virtual void Visit(class Name *) = 0;
  virtual void Visit(class UnaryOpExpr *) = 0;
  virtual void Visit(class BinaryOpExpr *) = 0;
  virtual void Visit(class LogicExpr *) = 0;
  virtual void Visit(class IfElseExpr *) = 0;
  virtual void Visit(class CallExpr *) = 0;
  virtual void Visit(class SubscriptExpr *) = 0;

  virtual void Visit(class CallOperator *) = 0;
  virtual void Visit(class SubscriptOperator *) = 0;

  virtual void Visit(class OpPlus *) = 0;
  virtual void Visit(class OpMinus *) = 0;
  virtual void Visit(class OpMul *) = 0;
  virtual void Visit(class OpDiv *) = 0;
  virtual void Visit(class OpMod *) = 0;
  virtual void Visit(class OpBitXor *) = 0;
  virtual void Visit(class OpBitOr *) = 0;
  virtual void Visit(class OpBitAnd *) = 0;
  virtual void Visit(class OpShiftL *) = 0;
  virtual void Visit(class OpShiftR *) = 0;

  virtual void Visit(class OpAssign *) = 0;
  virtual void Visit(class OpSelfPlus *) = 0;
  virtual void Visit(class OpSelfMinus *) = 0;
  virtual void Visit(class OpSelfMul *) = 0;
  virtual void Visit(class OpSelfDiv *) = 0;
  virtual void Visit(class OpSelfMod *) = 0;
  virtual void Visit(class OpSelfBitXor *) = 0;
  virtual void Visit(class OpSelfBitOr *) = 0;
  virtual void Visit(class OpSelfBitAnd *) = 0;
  virtual void Visit(class OpSelfShiftL *) = 0;
  virtual void Visit(class OpSelfShiftR *) = 0;

  virtual void Visit(class OpOr *) = 0;
  virtual void Visit(class OpAnd *) = 0;
  virtual void Visit(class OpEq *) = 0;
  virtual void Visit(class OpNe *) = 0;
  virtual void Visit(class OpLe *) = 0;
  virtual void Visit(class OpGe *) = 0;
  virtual void Visit(class OpLt *) = 0;
  virtual void Visit(class OpGt *) = 0;

  virtual void Visit(class OpBitNot *) = 0;
  virtual void Visit(class OpNot *) = 0;
  virtual void Visit(class OpPositive *) = 0;
  virtual void Visit(class OpNegative *) = 0;
  virtual void Visit(class OpDeref *) = 0;
  virtual void Visit(class OpRef *) = 0;
};

}  // namespace ast

#endif  // _XULANG_SRC_AST_VISITOR_HPP

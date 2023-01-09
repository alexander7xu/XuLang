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
  virtual void Visit(class AssignOpExpr *) = 0;
  virtual void Visit(class LogicExpr *) = 0;
  virtual void Visit(class IfElseExpr *) = 0;
  virtual void Visit(class CallExpr *) = 0;
  virtual void Visit(class SubscriptExpr *) = 0;

  virtual void Visit(class CallOperator *) = 0;
  virtual void Visit(class SubscriptOperator *) = 0;

  virtual void Visit(class OpPlus *);
  virtual void Visit(class OpMinus *);
  virtual void Visit(class OpMul *);
  virtual void Visit(class OpDiv *);
  virtual void Visit(class OpMod *);
  virtual void Visit(class OpBitXor *);
  virtual void Visit(class OpBitOr *);
  virtual void Visit(class OpBitAnd *);
  virtual void Visit(class OpShiftL *);
  virtual void Visit(class OpShiftR *);

  virtual void Visit(class OpAssign *);
  virtual void Visit(class OpSelfPlus *);
  virtual void Visit(class OpSelfMinus *);
  virtual void Visit(class OpSelfMul *);
  virtual void Visit(class OpSelfDiv *);
  virtual void Visit(class OpSelfMod *);
  virtual void Visit(class OpSelfBitXor *);
  virtual void Visit(class OpSelfBitOr *);
  virtual void Visit(class OpSelfBitAnd *);
  virtual void Visit(class OpSelfShiftL *);
  virtual void Visit(class OpSelfShiftR *);

  virtual void Visit(class OpOr *);
  virtual void Visit(class OpAnd *);
  virtual void Visit(class OpEq *);
  virtual void Visit(class OpNe *);
  virtual void Visit(class OpLe *);
  virtual void Visit(class OpGe *);
  virtual void Visit(class OpLt *);
  virtual void Visit(class OpGt *);

  virtual void Visit(class OpBitNot *);
  virtual void Visit(class OpNot *);
  virtual void Visit(class OpPositive *);
  virtual void Visit(class OpNegative *);
  virtual void Visit(class OpDeref *);
  virtual void Visit(class OpRef *);
};

}  // namespace ast

#endif  // _XULANG_SRC_AST_VISITOR_HPP

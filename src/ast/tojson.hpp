#ifndef _XULANG_SRC_AST_TOJSON_HPP
#define _XULANG_SRC_AST_TOJSON_HPP

#include "./statement.hpp"

namespace ast {

class ToJson final : private VisitorInterface {
 private:
  std::string _visit_result;

  template <class LeafP>
  std::string JsonPair(const std::string &key, const LeafP &val) {
    auto res = std::string("\"") + key + "\":";
    if (val == nullptr) return res + "\"NULL\"";
    val->Accept(this);
    auto child = this->GetVisitResult();
    res += (child[0] == '{' || child[0] == '[') ? child : ('"' + child + '"');
    return res;
  }

  std::string JsonPair(const std::string &key,
                       const Uptr<builtin::BasicType> &val) {
    return std::string("\"") + key + "\":\"" + val->GetName() + "\"";
  }

  std::string JsonPair(const std::string &key, const Uptr<TextType> &val) {
    return std::string("\"") + key + "\":\"" + *val + "\"";
  }

  std::string JsonPair(const std::string &key, const std::string &val) {
    auto res = std::string("\"") + key + "\":";
    res += (val[0] == '{' || val[0] == '[') ? val : ('"' + val + '"');
    return res;
  }

  std::string JsonPair(const std::string &key, const bool &val) {
    return std::string("\"") + key + "\":\"" + (val ? "True" : "False") + "\"";
  }

  const std::string &GetVisitResult() const { return _visit_result; }

 public:
  std::string operator()(const Node *node) {
    const_cast<Node *>(node)->Accept(this);
    return GetVisitResult();
  }

 private:
  virtual void Visit(Module *module) override {
    auto objs = std::string("[");
    for (const auto &obj : module->objs) {
      obj->Accept(this);
      objs += this->GetVisitResult() + ",";
    }

    if (objs.length() > 1) {
      objs[objs.length() - 1] = ']';
    } else {
      objs += "]";
    }

    auto res = std::string("{");
    res += JsonPair("class", std::string("Module")) + ",";
    res += JsonPair("filename", module->filename) + ",";
    res += JsonPair("objs", objs);
    this->_visit_result = res + "}";
  }

  virtual void Visit(Block *block) override {
    auto stmts = std::string("[");
    for (const auto &stmt : block->statements) {
      stmt->Accept(this);
      stmts += this->GetVisitResult() + ",";
    }

    if (stmts.length() > 1) {
      stmts[stmts.length() - 1] = ']';
    } else {
      stmts += "]";
    }

    auto res = std::string("{");
    res += JsonPair("class", std::string("Block")) + ",";
    res += JsonPair("statements", stmts);
    this->_visit_result = res + "}";
  }

  virtual void Visit(Try *try_stmt) override {
    auto excepts = std::string("[");
    for (const auto &x : try_stmt->excepts) {
      excepts += "{";
      excepts += JsonPair("alias", std::get<0>(x)) + ",";
      excepts += JsonPair("error", std::get<1>(x)) + ",";
      excepts += JsonPair("body", std::get<2>(x));
      excepts += "},";
    }
    excepts[excepts.length() - 1] = ']';

    auto res = std::string("{");
    res += JsonPair("class", std::string("Try")) + ",";
    res += JsonPair("body", try_stmt->body) + ",";
    res += JsonPair("excepts", excepts) + ",";
    res += JsonPair("orelse", try_stmt->orelse);
    _visit_result = res + "}";
  }

  virtual void Visit(CallOperator *cop) override {
    auto unamed = std::string("[");
    for (const auto &x : cop->unameds) {
      x->Accept(this);
      unamed += this->GetVisitResult() + ",";
    }
    if (unamed.length() > 1) {
      unamed[unamed.length() - 1] = ']';
    } else {
      unamed += "]";
    }

    auto named = std::string("{");
    for (const auto &x : cop->keywords) {
      named += JsonPair(*std::get<0>(x), std::get<1>(x)) + ",";
    }
    if (named.length() > 1) {
      named[named.length() - 1] = '}';
    } else {
      named += "}";
    }

    auto res = std::string("{");
    res += JsonPair("class", std::string("CallOperator")) + ",";
    res += JsonPair("name", cop->GetName()) + ",";
    res += JsonPair("unamed", unamed) + ",";
    res += JsonPair("keywords", named);
    _visit_result = res + "}";
  }

  virtual void Visit(SubscriptOperator *sop) override {
    auto dims = std::string("[");
    for (const auto &x : sop->dims) {
      dims += "{";
      dims += JsonPair("beg", std::get<0>(*x)) + ",";
      dims += JsonPair("end", std::get<1>(*x)) + ",";
      dims += JsonPair("step", std::get<2>(*x));
      dims += "},";
    }
    dims[dims.length() - 1] = ']';

    auto res = std::string("{");
    res += JsonPair("class", std::string("SubscriptOperator")) + ",";
    res += JsonPair("dims", dims);
    _visit_result = res + "}";
  }

#define _ADD_JSON_PAIR(name) (res += JsonPair(#name, leaf->name) + ",");
#define _LEAF_TO_JSON_FUNC(LeafT, ...)                     \
  virtual void Visit(LeafT *leaf) override {               \
    auto res = std::string("{");                           \
    res += JsonPair("class", std::string(#LeafT)) + ",";   \
    FOR_EACH(_ADD_JSON_PAIR, __VA_ARGS__);                 \
    _visit_result = res.substr(0, res.length() - 1) + "}"; \
  }
#define _OP_LEAF_TO_JSON_FUNC(OpLeafT)                             \
  virtual void Visit(OpLeafT *leaf) override {                     \
    auto res = std::string("{");                                   \
    res += JsonPair("class", std::string(#OpLeafT)) + ",";         \
    _visit_result = res + JsonPair("name", leaf->GetName()) + "}"; \
  }

  _LEAF_TO_JSON_FUNC(ExprStatement, expr)
  _LEAF_TO_JSON_FUNC(Break)
  _LEAF_TO_JSON_FUNC(Continue)
  _LEAF_TO_JSON_FUNC(Return, expr)
  _LEAF_TO_JSON_FUNC(If, test, body, orelse)
  _LEAF_TO_JSON_FUNC(While, test, body, orelse)
  _LEAF_TO_JSON_FUNC(ObjCreate, id, call_expr)
  _LEAF_TO_JSON_FUNC(Function, id, args, body)
  _LEAF_TO_JSON_FUNC(Assemble, id, args, body)
  _LEAF_TO_JSON_FUNC(Struct, id, body)
  _LEAF_TO_JSON_FUNC(Class, id, parents, body)
  _LEAF_TO_JSON_FUNC(Import, id, module_root, files)
  _LEAF_TO_JSON_FUNC(Raise, error)

  _LEAF_TO_JSON_FUNC(Literal, val, type)
  _LEAF_TO_JSON_FUNC(Name, id, deref, parent)
  _LEAF_TO_JSON_FUNC(UnaryOpExpr, op, right)
  _LEAF_TO_JSON_FUNC(BinaryOpExpr, op, left, right)
  _LEAF_TO_JSON_FUNC(LogicExpr, op, left, right)
  _LEAF_TO_JSON_FUNC(IfElseExpr, test, left, right)
  _LEAF_TO_JSON_FUNC(CallExpr, obj, op)
  _LEAF_TO_JSON_FUNC(SubscriptExpr, obj, op)

  _OP_LEAF_TO_JSON_FUNC(OpPlus)
  _OP_LEAF_TO_JSON_FUNC(OpMinus)
  _OP_LEAF_TO_JSON_FUNC(OpMul)
  _OP_LEAF_TO_JSON_FUNC(OpDiv)
  _OP_LEAF_TO_JSON_FUNC(OpMod)
  _OP_LEAF_TO_JSON_FUNC(OpBitXor)
  _OP_LEAF_TO_JSON_FUNC(OpBitOr)
  _OP_LEAF_TO_JSON_FUNC(OpBitAnd)
  _OP_LEAF_TO_JSON_FUNC(OpShiftL)
  _OP_LEAF_TO_JSON_FUNC(OpShiftR)

  _OP_LEAF_TO_JSON_FUNC(OpAssign)
  _OP_LEAF_TO_JSON_FUNC(OpSelfPlus)
  _OP_LEAF_TO_JSON_FUNC(OpSelfMinus)
  _OP_LEAF_TO_JSON_FUNC(OpSelfMul)
  _OP_LEAF_TO_JSON_FUNC(OpSelfDiv)
  _OP_LEAF_TO_JSON_FUNC(OpSelfMod)
  _OP_LEAF_TO_JSON_FUNC(OpSelfBitXor)
  _OP_LEAF_TO_JSON_FUNC(OpSelfBitOr)
  _OP_LEAF_TO_JSON_FUNC(OpSelfBitAnd)
  _OP_LEAF_TO_JSON_FUNC(OpSelfShiftL)
  _OP_LEAF_TO_JSON_FUNC(OpSelfShiftR)

  _OP_LEAF_TO_JSON_FUNC(OpOr)
  _OP_LEAF_TO_JSON_FUNC(OpAnd)
  _OP_LEAF_TO_JSON_FUNC(OpEq)
  _OP_LEAF_TO_JSON_FUNC(OpNe)
  _OP_LEAF_TO_JSON_FUNC(OpLe)
  _OP_LEAF_TO_JSON_FUNC(OpGe)
  _OP_LEAF_TO_JSON_FUNC(OpLt)
  _OP_LEAF_TO_JSON_FUNC(OpGt)

  _OP_LEAF_TO_JSON_FUNC(OpBitNot)
  _OP_LEAF_TO_JSON_FUNC(OpNot)
  _OP_LEAF_TO_JSON_FUNC(OpPositive)
  _OP_LEAF_TO_JSON_FUNC(OpNegative)
  _OP_LEAF_TO_JSON_FUNC(OpDeref)
  _OP_LEAF_TO_JSON_FUNC(OpRef)
};

};  // namespace ast

#endif  // _XULANG_SRC_AST_TOJSON_HPP

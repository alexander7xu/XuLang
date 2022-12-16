#ifndef _XULANG_SRC_AST_STATEMENT_HPP
#define _XULANG_SRC_AST_STATEMENT_HPP

#include "./expression.hpp"

namespace ast {

class Statement : public Node {};
class Create : public Statement {};

class Module final : public Statement {
 public:
  ast::TextType filename;
  std::list<Uptr<Create>> objs;
  Module(const ast::TextType &filename) : filename(filename) {}
  Module(const ast::TextType &filename, Uptr<Create> &&obj)
      : filename(filename) {
    AddObj(std::move(obj));
  }
  virtual void Accept(VisitorInterface *) override;

  inline void AddObj(Uptr<Create> &&obj) { objs.push_back(std::move(obj)); }
};

class Block final : public Statement {
 public:
  std::list<Uptr<Statement>> statements;
  Block() = default;
  Block(Uptr<Statement> &&statement) { AddStatement(std::move(statement)); }
  virtual void Accept(VisitorInterface *) override;

  inline void AddStatement(Uptr<Statement> &&statement) {
    statements.push_back(std::move(statement));
  }
};

// e.g. +7 , different from Expression
class ExprStatement final : public Statement {
 public:
  Uptr<Expression> expr;
  ExprStatement(Uptr<Expression> &&expr) : expr(std::move(expr)) {}
  virtual void Accept(VisitorInterface *) override;
};

class Break final : public Statement {
  virtual void Accept(VisitorInterface *) override;
};

class Continue final : public Statement {
  virtual void Accept(VisitorInterface *) override;
};

class Return final : public Statement {
 public:
  Uptr<Expression> expr;
  Return(Uptr<Expression> &&expr = nullptr) : expr(std::move(expr)) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. if (test) {  } else if (exp_b) { } else { }
class If final : public Statement {
 public:
  Uptr<Expression> test;
  Uptr<Block> body;
  Uptr<Block> orelse;
  If(Uptr<Expression> &&test, Uptr<Block> &&body,
     Uptr<Block> &&orelse = nullptr)
      : test(std::move(test)),
        body(std::move(body)),
        orelse(std::move(orelse)) {}
  virtual void Accept(VisitorInterface *) override;

  inline void SetOrelse(Uptr<Block> &&block) { orelse = std::move(block); }
};

// e.g. while (test) {  } else { }
class While final : public Statement {
 public:
  Uptr<Expression> test;
  Uptr<Block> body;
  Uptr<Block> orelse;
  While(Uptr<Expression> &&test, Uptr<Block> &&body,
        Uptr<Block> &&orelse = nullptr)
      : test(std::move(test)),
        body(std::move(body)),
        orelse(std::move(orelse)) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. obj_name := Type(expr)
class ObjCreate final : public Create {
 public:
  Uptr<TextType> id;
  Uptr<CallExpr> call_expr;
  ObjCreate(Uptr<TextType> &&id, Uptr<CallExpr> &&call_expr)
      : id(std::move(id)), call_expr(std::move(call_expr)) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. func_name := Function(Void, Arg0:=T0(), Arg1:=T1()) { }
class Function final : public Create {
 public:
  Uptr<TextType> id;
  Uptr<CallOperator> args;
  Uptr<Block> body;
  Function(Uptr<TextType> &&id, Uptr<CallOperator> &&args, Uptr<Block> &&body)
      : id(std::move(id)), args(std::move(args)), body(std::move(body)) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. func_name := Function(Void, Arg0:=T0(), Arg1:=T1()) { }
class Assemble final : public Create {
 public:
  Uptr<TextType> id;
  Uptr<CallOperator> args;
  Uptr<Block> body;
  Assemble(Uptr<TextType> &&id, Uptr<CallOperator> &&args, Uptr<Block> &&body)
      : id(std::move(id)), args(std::move(args)), body(std::move(body)) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. StructName := Struct { }
class Struct final : public Create {
 public:
  Uptr<TextType> id;
  Uptr<Block> body;
  Struct(Uptr<TextType> &&id, Uptr<Block> &&body)
      : id(std::move(id)), body(std::move(body)) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. ClassName := Class(Base0, Base1) { }
class Class final : public Create {
 public:
  Uptr<TextType> id;
  Uptr<CallOperator> parents;
  Uptr<Block> body;
  Class(Uptr<TextType> &&id, Uptr<CallOperator> &&parents, Uptr<Block> &&body)
      : id(std::move(id)), parents(std::move(parents)), body(std::move(body)) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. AliasName := Import(ModuleRoot) { FileA, FileB }
class Import final : public Create {
 public:
  Uptr<TextType> id;
  Uptr<CallOperator> module_root;
  Uptr<Block> files;
  Import(Uptr<TextType> &&id, Uptr<CallOperator> &&module_root,
         Uptr<Block> &&files)
      : id(std::move(id)),
        module_root(std::move(module_root)),
        files(std::move(files)) {}
  virtual void Accept(VisitorInterface *) override;
};

class Raise final : public Statement {
 public:
  Uptr<Expression> error;
  Raise(Uptr<Expression> &&error) : error(std::move(error)) {}
  virtual void Accept(VisitorInterface *) override;
};

// e.g. try {} except (err := Error1()) {} except (err := Error2()) {} else {}
class Try final : public Statement {
 public:
  Uptr<Block> body;
  std::list<std::tuple<Uptr<TextType>, Uptr<Name>, Uptr<Block>>> excepts;
  Uptr<Block> orelse;
  Try(Uptr<Block> &&body, Uptr<Block> &&orelse = nullptr)
      : body(std::move(body)), orelse(std::move(orelse)) {}
  virtual void Accept(VisitorInterface *) override;

  inline void SetOrelse(Uptr<Block> &&block) { orelse = std::move(block); }
  inline void AddExcept(
      std::tuple<Uptr<TextType>, Uptr<Name>, Uptr<Block>> &&except) {
    excepts.push_back(std::move(except));
  }
};

}  // namespace ast

#endif  // _XULANG_SRC_AST_STATEMENT_HPP

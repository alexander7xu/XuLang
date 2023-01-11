#ifndef _XULANG_SRC_CHECK_CHECK_HPP
#define _XULANG_SRC_CHECK_CHECK_HPP

#include <unordered_set>

#include "../ast/statement.hpp"
#include "./context.hpp"

namespace check {

class Check final : private ast::VisitorInterface {
  struct {
    int status = -1;
    TypeSymbol *expr_type = nullptr;
    Symbol *last_symbol = nullptr;
    ThreeAddrCode *three_addr_code = nullptr;
    std::list<std::pair<const ast::Statement &, ThreeAddrCode *>>
        ctrl_flow_codes;
  } _result;

  Context _context;

  struct PrettyResult {
    // name, type, block_id
    std::forward_list<std::tuple<std::string, std::string, std::string>>
        symbols;
    // op, left, right, target, code_id
    std::forward_list<ThreeAddrCode> codes;
  } _pretty_result;

 public:
  inline bool operator()(ast::Module &module) {
    module.Accept(this);
    return _result.status == 0;
  }

  inline PrettyResult &MakePrettyResult() {
    MakePrettyResult(_context.GetCurrentBlock());

    for (auto &code : _context.GetThreeAddressCodes()) {
      _pretty_result.codes.push_front(code);
    }
    return _pretty_result;
  }

 private:
  void MakePrettyResult(decltype(_context.GetCurrentBlock()) &block,
                        const std::string &bid = "") {
    for (auto &child : block.children) {
      MakePrettyResult(child, bid + "." + std::to_string(child.id));
    }
    for (auto &obj : block.symbol_map) {
      auto &symbol = obj.second;
      auto &name = symbol->GetName();
      auto &type = symbol->GetType().GetName();
      _pretty_result.symbols.push_front({name, type, bid.length() ? bid : "."});
    }
  }

  template <class AstStatmentT>
  auto ExtractCtrlFlowCodes() {
    decltype(_result.ctrl_flow_codes) res;

    static const auto copy_ctrl_flow_stmts =
        [&res](decltype(_result.ctrl_flow_codes.front()) &pair) -> bool {
      if (typeid(pair.first) == typeid(AstStatmentT)) {
        res.push_back(pair);
        return true;
      }
      return false;
    };

    _result.ctrl_flow_codes.remove_if(copy_ctrl_flow_stmts);
    return res;
  }

  virtual void Visit(ast::Module *) override;
  virtual void Visit(ast::Block *) override;
  virtual void Visit(ast::ExprStatement *) override;
  virtual void Visit(ast::Break *) override;
  virtual void Visit(ast::Continue *) override;
  virtual void Visit(ast::Return *) override;
  virtual void Visit(ast::If *) override;
  virtual void Visit(ast::While *) override;
  virtual void Visit(ast::ObjCreate *) override;
  virtual void Visit(ast::Function *) override;
  virtual void Visit(ast::Assemble *) override;
  virtual void Visit(ast::Struct *) override;
  virtual void Visit(ast::Class *) override;
  virtual void Visit(ast::Import *) override;
  virtual void Visit(ast::Raise *) override;
  virtual void Visit(ast::Try *) override;

  virtual void Visit(ast::Literal *) override;
  virtual void Visit(ast::Name *) override;
  virtual void Visit(ast::UnaryOpExpr *) override;
  virtual void Visit(ast::BinaryOpExpr *) override;
  virtual void Visit(ast::AssignOpExpr *) override;
  virtual void Visit(ast::LogicExpr *) override;
  virtual void Visit(ast::IfElseExpr *) override;
  virtual void Visit(ast::CallExpr *) override;
  virtual void Visit(ast::SubscriptExpr *) override;

  virtual void Visit(ast::CallOperator *) override {}
  virtual void Visit(ast::SubscriptOperator *) override {}
};

};  // namespace check

#endif  // _XULANG_SRC_CHECK_CHECK_HPP

#include "./check.hpp"

namespace check {

void Check::Visit(ast::Literal *literal) {
  kLog->Debug({"Checking literal"});
  _result.status = -1;

  TypeSymbol *type = nullptr;
  if (literal->type == ast::Literal::Type::kInt) {
    type = builtin_type_symbol::kInt.get();
  } else if (literal->type == ast::Literal::Type::kFloat) {
    type = builtin_type_symbol::kFloat.get();
  } else if (literal->type == ast::Literal::Type::kString) {
    type = builtin_type_symbol::kString.get();
  } else {
    return kLog->Critical({SRC_LOC, "Unknown builtin type:",
                           ast::Literal::kTypeNames[literal->type]});
  }

  auto code = &_context.AddThreeAddressCode("__literal__", *literal->val,
                                            type->GetName());

  _result.three_addr_code = code;
  _result.expr_type = type;
  _result.status = 0;
  return kLog->Debug({"Done checking literal"});
}

void Check::Visit(ast::Name *name) {
  kLog->Debug({"Checking name"});
  _result.status = -1;

  if (name->parent != nullptr) {
    return kLog->Critical({SRC_LOC, "Name parent currently unsupported"});
  }

  auto symbol = _context.FindSymbol(*name->id);
  if (symbol == nullptr) {
    return kLog->Error({"Undefined symbol:", *name->id});
  }

  _result.three_addr_code = nullptr;
  auto op =
      dynamic_cast<TypeSymbol *>(symbol) != nullptr ? "__type__" : "__load__";
  auto code = &_context.AddThreeAddressCode(
      op, symbol->GetName() + symbol->GetBlockName(), "");

  _result.three_addr_code = code;
  _result.last_symbol = symbol;
  _result.expr_type = &symbol->GetType();
  _result.status = 0;
  return kLog->Debug({"Done checking name"});
}

void Check::Visit(ast::BinaryOpExpr *bop_expr) {
  kLog->Debug({"Checking bop-expr"});
  _result.status = -1;

  bop_expr->left->Accept(this);
  if (_result.status != 0) return;
  auto l_type = _result.expr_type;
  auto l_code = _result.three_addr_code;

  bop_expr->right->Accept(this);
  if (_result.status != 0) return;
  auto r_type = _result.expr_type;
  auto r_code = _result.three_addr_code;

  if (l_type != builtin_type_symbol::kInt.get() ||
      r_type != builtin_type_symbol::kInt.get()) {
    _result.status = -1;
    return kLog->Critical(
        {SRC_LOC, "Currently only support Int __bop__ Int, got"});
  }

  auto code = &_context.AddThreeAddressCode(bop_expr->op->GetName(),
                                            l_code->res, r_code->res);

  _result.three_addr_code = code;
  _result.expr_type = builtin_type_symbol::kInt.get();
  _result.status = 0;
  return kLog->Debug({"Done checking bop-expr"});
}

void Check::Visit(ast::AssignOpExpr *aop_expr) {
  kLog->Debug({"Checking assign-expr"});
  _result.status = -1;

  aop_expr->expr->Accept(this);
  if (_result.status != 0) return;
  auto r_type = _result.expr_type;
  auto r_code = _result.three_addr_code;

  aop_expr->target->Accept(this);
  if (_result.status != 0) return;
  auto l_type = _result.expr_type;
  auto l_code = _result.three_addr_code;

  if (l_type != builtin_type_symbol::kInt.get() ||
      r_type != builtin_type_symbol::kInt.get()) {
    _result.status = -1;
    return kLog->Critical({SRC_LOC, "Currently only support Int __aop__ Int"});
  }

  if (dynamic_cast<ast::OpAssign *>(aop_expr->op.get()) != nullptr) {
    // ("__load__", name, "", "tL")  ==> ("__store__", "tR", "", name)
    l_code->op = "__store__";
    l_code->res = l_code->left;
    l_code->left = r_code->res;
    l_code->right = "";
  } else {
    // ("__load__", name, "", "tL")  ==> ("__bop__", name, "tR", name)
    l_code->op = aop_expr->op->GetName();
    l_code->res = l_code->left;
    l_code->right = r_code->res;
  }

  _result.three_addr_code = l_code;
  _result.expr_type = builtin_type_symbol::kInt.get();
  _result.status = 0;

  return kLog->Debug({"Done checking assign-expr"});
}

void Check::Visit(ast::LogicExpr *logic_expr) {
  // A+0: (..., "tL")                     # calculate tL
  // B+0: ("__jf__", "tL", "", "B+3")     # goto logic_rt if tL is False
  // B+1: ("__loc__", "", "", "logic_lt") #
  // B+2: ("__load__", "tL", "", "tT")    # tT = tL
  // B+3: ("__jp__", "", "", "C+1")       # goto logic_ed
  // B+4: ("__loc__", "", "", "logic_rt") #
  // B+5: (..., "tR")                     # calculate tR
  // C+0: ("__load__", "tR", "", "tT")    # tT = tR
  // C+1: ("__loc__", "", "", "logic_ed") #

  // A+0: (..., "tL")                     # calculate tL
  // B+0: ("__jt__", "tL", "", "B+3")     # goto logic_rt if tL is True
  // B+1: ("__loc__", "", "", "logic_lt") #
  // B+2: ("__load__", "tL", "", "tT")    # tT = tL
  // B+3: ("__jp__", "", "", "C+1")       # goto logic_ed
  // B+4: ("__loc__", "", "", "logic_rt") #
  // B+5: (..., "tR")                     # calculate tR
  // C+0: ("__load__", "tR", "", "tT")    # tT = tR
  // C+1: ("__loc__", "", "", "logic_ed") #

  // __other__
  // A+0: (..., "tL")                     # calculate tL
  // B+0: (..., "tR")                     # calculate tR
  // C+0: ("__cmp__", "tL", "tR", "tT")   # compare tL and tR

  kLog->Debug({"Checking logic-expr"});
  _result.status = -1;

  logic_expr->left->Accept(this);  // calculate tL
  if (_result.status != 0) return;
  auto l_type = _result.expr_type;
  auto l_code = _result.three_addr_code;
  decltype(l_code) r_code = nullptr, code = nullptr;

  if (dynamic_cast<ast::OpOr *>(logic_expr->op.get()) != nullptr ||
      dynamic_cast<ast::OpAnd *>(logic_expr->op.get()) != nullptr) {
    auto jmp_op = dynamic_cast<ast::OpOr *>(logic_expr->op.get()) != nullptr
                      ? "__jf__"
                      : "__jt__";
    auto jmpc2right =  // B+0
        &_context.AddThreeAddressCode(jmp_op, l_code->res, "", "logic_rt");
    _context.AddThreeAddressCode("__loc__", "", "", "logic_lt");  // B+1

    auto load_left =  // B+2
        &_context.AddThreeAddressCode("__copytmp__", l_code->res, "");
    const auto &target_tmp = load_left->res;  // tT
    auto jmp2ed =                             // B+3
        &_context.AddThreeAddressCode("__jp__", "", "", "?");

    auto loc_right =  // B+4
        &_context.AddThreeAddressCode("__loc__", "", "", "logic_rt");
    jmpc2right->res = loc_right->id;

    logic_expr->right->Accept(this);  // calculate tR
    if (_result.status != 0) return;
    auto r_type = _result.expr_type;
    r_code = _result.three_addr_code;

    if (l_type != r_type) {
      _result.status = -1;
      return kLog->Error({"Except operands of", logic_expr->op->GetName(),
                          "to be with same type, got", l_type->GetName(),
                          r_type->GetName()});
    }

    // auto load_right = & // C+0
    _context.AddThreeAddressCode("__copytmp__", r_code->res, "", target_tmp);
    auto loc_end =  // C+1
        &_context.AddThreeAddressCode("__loc__", "", "", "logic_ed");
    jmp2ed->res = loc_end->id;

    code = load_left;
  } else {
    logic_expr->right->Accept(this);  // calculate tR
    if (_result.status != 0) return;
    auto r_type = _result.expr_type;
    r_code = _result.three_addr_code;

    if (l_type != builtin_type_symbol::kInt.get() ||
        r_type != builtin_type_symbol::kInt.get()) {
      _result.status = -1;
      return kLog->Critical({SRC_LOC, "Only Int __cmp__ Int supported"});
    }
    code =  // C+0
        &_context.AddThreeAddressCode(logic_expr->op->GetName(), l_code->res,
                                      r_code->res);
  }

  _result.expr_type = l_type;
  _result.three_addr_code = code;
  _result.status = 0;
  return kLog->Debug({"Done checking logic-expr"});
}

void Check::Visit(ast::UnaryOpExpr *) {
  return kLog->Critical({SRC_LOC, "Unimplemented"});
}
void Check::Visit(ast::IfElseExpr *) {
  return kLog->Critical({SRC_LOC, "Unimplemented"});
}
void Check::Visit(ast::CallExpr *) {
  return kLog->Critical({SRC_LOC, "Unimplemented"});
}
void Check::Visit(ast::SubscriptExpr *) {
  return kLog->Critical({SRC_LOC, "Unimplemented"});
}

}  // namespace check

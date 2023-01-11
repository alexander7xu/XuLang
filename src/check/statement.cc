#include "./check.hpp"

namespace check {

void Check::Visit(ast::Module *module) {
  kLog->Debug({"Checking module:", module->filename});
  _result.status = -1;

  _context.AddThreeAddressCode("__begmodule__", "", "", module->filename);
  _context.PushBlock();

  for (auto &obj : module->objs) {
    obj->Accept(this);
    if (_result.status != 0) return;
  }
  _context.PopBlock();
  _context.AddThreeAddressCode("__endmodule__", "", "", module->filename);

  _result.status = 0;
  return kLog->Debug({"Done checking module:", module->filename});
}

void Check::Visit(ast::Block *block) {
  _context.PushBlock();
  auto bid = _context.GetBlockId();
  kLog->Debug({"Checking block:", bid});

  _result.status = -1;
  for (auto &stmt : block->statements) {
    stmt->Accept(this);
    if (_result.status != 0) return;
  }
  _result.status = 0;

  _context.PopBlock();
  return kLog->Debug({"Done checking block:", bid});
}

void Check::Visit(ast::ExprStatement *expr_stmt) {
  expr_stmt->expr->Accept(this);
}

void Check::Visit(ast::ObjCreate *create) {
  kLog->Debug({"Checking create-stmt"});
  _result.status = -1;

  create->expr->Accept(this);
  if (_result.status != 0) return;

  auto symbol = &_context.AddSymbol(
      ObjectSymbol::New(create->GetId(), *_result.expr_type));
  auto code = &_context.AddThreeAddressCode(
      "__create__", _result.three_addr_code->res, "",
      symbol->GetName() + symbol->GetBlockName());

  _result.expr_type = nullptr;
  _result.three_addr_code = code;
  _result.status = 0;
  return kLog->Debug({"Done checking create-stmt"});
}

void Check::Visit(ast::Function *func) {
  kLog->Debug({"Checking function"});

  _result.status = -1;

  if (func->args->unameds.size() != 1) {
    return kLog->Error(
        {"Function must defined with one unamed arg as return type"});
  }
  if (func->args->keywords.size() != 0) {
    return kLog->Critical({SRC_LOC, "Default args currently unsupported"});
  }

  auto bid = func->GetId() + '@' + _context.GetBlockId();
  _context.AddThreeAddressCode("__begfunc__", "", "", bid);
  _context.PushBlock();

  _result.last_symbol = nullptr;
  func->args->unameds.front()->Accept(this);
  if (_result.status != 0) return;
  auto ret_type = dynamic_cast<TypeSymbol *>(_result.last_symbol);
  if (ret_type == nullptr) {
    _result.status = -1;
    return kLog->Error(
        {"Arg0 of function declation except to be a TypeSymbol"});
  }

  auto func_symbol =
      &_context.AddSymbol(FunctionSymbol::New(func->GetId(), *ret_type));

  for (auto &arg : func->args->type_args) {
    auto &arg_name = std::get<0>(arg);
    auto &type_name = std::get<1>(arg);
    type_name->Accept(this);
    if (_result.status != 0) return;
    auto type = dynamic_cast<TypeSymbol *>(_result.last_symbol);
    if (type == nullptr) {
      _result.status = -1;
      return kLog->Error({"Arg1 Type except to be a TypeSymbol"});
    }
    auto sym = &_context.AddSymbol(ObjectSymbol::New(*arg_name, *type));
    _context.AddThreeAddressCode("__arg__", "",
                                 type->GetName() + type->GetBlockName(),
                                 sym->GetName() + sym->GetBlockName());
  }

  _context.AddThreeAddressCode("__begfunc_body__", "", "", bid);

  decltype(ExtractCtrlFlowCodes<ast::Return>()) ret_stmts;
  for (auto &stmt : func->body->statements) {
    stmt->Accept(this);

    for (auto &ret : ExtractCtrlFlowCodes<ast::Return>()) ret.second->res = bid;
    // TODO: Parse return stmt here
    if (_result.ctrl_flow_codes.size() != 0) {
      _result.status = -1;
      return kLog->Error({"Control flow statement not allowed here"});
    }
  }

  _context.PopBlock();
  auto end_func = &_context.AddThreeAddressCode("__endfunc__", "", "", bid);

  _result.last_symbol = func_symbol;
  _result.three_addr_code = end_func;
  _result.expr_type = nullptr;
  _result.status = 0;
  return kLog->Debug({"Done checking function"});
}

void Check::Visit(ast::If *if_stmt) {
  // A-1: ("__loc__", "", "", "begif")    #
  // A+0: (..., "tTest")                  # calculate tTest
  // B+0: ("__jf__", "tTest", "", "C+1")  # goto else if tTest is False
  // B+1: ("__loc__", "", "", "then")     #
  // B+2: (..., "tBody")                  # calculate tBody
  // C+0: ("__jp__", "", "", "D+0")       # goto endif
  // C+1: ("__loc__", "", "", "else")     #
  // C+2: (..., "tOrelse")                # caculate tOrelse
  // D+0: ("__jp__", "", "", "D+1")       # goto endif
  // D+1: ("__loc__", "", "", "endif")    #

  kLog->Debug({"Checking if-stmt"});
  _result.status = -1;

  if (if_stmt->orelse != nullptr &&
      dynamic_cast<ast::If *>(if_stmt->orelse.get()) == nullptr &&
      dynamic_cast<ast::Block *>(if_stmt->orelse.get()) == nullptr) {
    return kLog->Critical(
        {"Except orelse statement to be ast::If or ast::Block, got",
         typeid(if_stmt->orelse.get()).name()});
  }

  // auto loc_beg = & // A+0
  // _context.AddThreeAddressCode("__loc__", "", "", "begif");
  if_stmt->test->Accept(this);  // calculate tTest
  if (_result.status != 0) return;

  auto test_code = _result.three_addr_code;
  auto jmpc2orelse =  // B+0
      &_context.AddThreeAddressCode("__jf__", test_code->res, "", "?");
  _context.AddThreeAddressCode("__loc__", "", "", "then");  // B+1

  if_stmt->body->Accept(this);  // calculate tBody
  if (_result.status != 0) return;

  auto jmp2end =  // C+0
      &_context.AddThreeAddressCode("__jp__", "", "", "?");
  auto loc_else =  // C+1
      &_context.AddThreeAddressCode("__loc__", "", "", "else");
  jmpc2orelse->res = loc_else->id;

  if (if_stmt->orelse != nullptr) {
    if_stmt->orelse->Accept(this);  // caculate tOrelse
    if (_result.status != 0) return;
  }

  auto orelse2end =  // D+0
      &_context.AddThreeAddressCode("__jp__", "", "", "?");
  auto loc_end =  // D+1
      &_context.AddThreeAddressCode("__loc__", "", "", "endif");
  orelse2end->res = jmp2end->res = loc_end->id;

  _result.three_addr_code = jmp2end;
  _result.expr_type = nullptr;
  _result.last_symbol = nullptr;
  _result.status = 0;
  return kLog->Debug({"Done checking if-stmt"});
}

void Check::Visit(ast::Break *brk) {
  kLog->Debug({"Checking break-stmt"});
  _result.status = -1;

  auto code = &_context.AddThreeAddressCode("__jp__", "", "", "?");
  _context.AddThreeAddressCode("__loc__", "", "", "break");
  _result.ctrl_flow_codes.push_back({*brk, code});

  _result.status = 0;
  return kLog->Debug({"Done checking break-stmt"});
}

void Check::Visit(ast::Continue *ctn) {
  kLog->Debug({"Checking continue-stmt"});
  _result.status = -1;

  auto code = &_context.AddThreeAddressCode("__jp__", "", "", "?");
  _context.AddThreeAddressCode("__loc__", "", "", "continue");
  _result.ctrl_flow_codes.push_back({*ctn, code});

  _result.status = 0;
  return kLog->Debug({"Done checking break-stmt"});
}

void Check::Visit(ast::Return *ret) {
  kLog->Debug({"Checking return-stmt"});
  _result.status = -1;

  ThreeAddrCode *ret_val = nullptr;
  if (ret->expr != nullptr) {
    ret->expr->Accept(this);
    ret_val = _result.three_addr_code;
  }

  auto code = &_context.AddThreeAddressCode(
      "__ret__", ret_val == nullptr ? "" : ret_val->res, "", "_");

  _result.ctrl_flow_codes.push_back({*ret, code});
  _result.three_addr_code = code;
  _result.expr_type = nullptr;
  _result.status = 0;
  return kLog->Debug({"Done checking return-stmt"});
}

void Check::Visit(ast::While *while_stmt) {
  // A-1: ("__loc__", "", "", "while")    #
  // A+0: ("__jp__", "", "", "C+1")       # goto test
  // A+1: ("__loc__", "", "", "do")       #
  // A+2: (..., "tBody_0")                # calculate tBody
  // B+0: ("__jp__", "", "", "E+1")       #   assume break: goto endwhile
  // B+1: ("__jp__", "", "", "C+1")       #   assume continue: goto test
  // B+2: (..., "tBody_1")                # calculate tBody
  // C+0: ("__jp__", "", "", "C+1")       # goto test
  // C+1: ("__loc__", "", "", "test")     #
  // C+2: (..., "tTest")                  # calculate tTest
  // D+0: ("__jt__", "", "", "A+1")       # goto body if tTest is true
  // D+1: ("__loc__", "", "", "else")     #
  // D+2: (..., "tOrelse")                # calculate orelse
  // E+0: ("__jp__", "", "", "E+1")       # goto endwhile
  // E+1: ("__loc__", "", "", "endwhile") #

  kLog->Debug({"Checking while-stmt"});
  _result.status = -1;

  // auto loc_beg = & // A-1
  // _context.AddThreeAddressCode("__loc__", "", "", "begwhile");
  auto jmp2test =  // A+0
      &_context.AddThreeAddressCode("__jp__", "", "", "?");
  auto loc_do =  // A+1
      &_context.AddThreeAddressCode("__loc__", "", "", "do");

  while_stmt->body->Accept(this);  // calculate tBody
  if (_result.status != 0) return;

  auto body2test =  // C+0
      &_context.AddThreeAddressCode("__jp__", "", "", "?");
  auto loc_test =  // C+1
      &_context.AddThreeAddressCode("__loc__", "", "", "test");
  body2test->res = jmp2test->res = loc_test->id;

  while_stmt->test->Accept(this);  // calculate tTest
  if (_result.status != 0) return;
  auto test_code = _result.three_addr_code;

  // auto jmp2body = & // D+0
  _context.AddThreeAddressCode("__jt__", test_code->res, "", loc_do->id);
  _context.AddThreeAddressCode("__loc__", "", "", "else");

  auto break_stmts = ExtractCtrlFlowCodes<ast::Break>();
  auto continue_stmts = ExtractCtrlFlowCodes<ast::Continue>();

  if (while_stmt->orelse != nullptr) {
    while_stmt->orelse->Accept(this);  // calculate tOresle
    if (_result.status != 0) return;
  }

  auto orelse2end = // E+0
      &_context.AddThreeAddressCode("__jp__", "", "", "?");
  auto loc_end =  // E+1
      &_context.AddThreeAddressCode("__loc__", "", "", "endwhile");
  orelse2end->res = loc_end->id;

  // Set jump target of break and continue statements
  for (auto &stmt : break_stmts) stmt.second->res = loc_end->id;
  for (auto &stmt : continue_stmts) stmt.second->res = loc_test->id;

  _result.three_addr_code = loc_end;
  _result.expr_type = nullptr;
  _result.last_symbol = nullptr;
  _result.status = 0;
  return kLog->Debug({"Done checking if-stmt"});
}

void Check::Visit(ast::Assemble *) {
  kLog->Critical({SRC_LOC, "Unimplemented"});
  _result.status = -1;
}
void Check::Visit(ast::Struct *) {
  kLog->Critical({SRC_LOC, "Unimplemented"});
  _result.status = -1;
}
void Check::Visit(ast::Class *) {
  kLog->Critical({SRC_LOC, "Unimplemented"});
  _result.status = -1;
}
void Check::Visit(ast::Import *) {
  kLog->Critical({SRC_LOC, "Unimplemented"});
  _result.status = -1;
}
void Check::Visit(ast::Raise *) {
  kLog->Critical({SRC_LOC, "Unimplemented"});
  _result.status = -1;
}
void Check::Visit(ast::Try *) {
  kLog->Critical({SRC_LOC, "Unimplemented"});
  _result.status = -1;
}

}  // namespace check

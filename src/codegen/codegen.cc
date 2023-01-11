#include "./codegen.hpp"

#include "../ast/statement.hpp"
#include "../utils/log.hpp"

namespace codegen {

static auto kLog = utils::Logger::New("codegen");
static llvm::LLVMContext kLlvmContext;
static llvm::Module *kLlvmModule = nullptr;
static llvm::BasicBlock *kLlvmBlock = nullptr;
static llvm::Function *kLlvmMain = nullptr;

static std::unordered_map<std::string, llvm::Value *> kName2Value;
static std::unordered_map<std::string, llvm::BasicBlock *> kLoc2Block;
static std::forward_list<llvm::Function *> kLlvmFunctions;
static std::list<std::tuple<const check::ThreeAddrCode &,
                            const check::ThreeAddrCode &, llvm::BasicBlock *>>
    kJmpCodes;

static int GenName(const check::ThreeAddrCode &code) {
  if (code.op != "__type__" && code.op != "__name__") return 1;
  return 0;
}

static int GenLiteral(const check::ThreeAddrCode &code) {
  if (code.op != "__literal__") return 1;
  if (code.right != "Int") {
    kLog->Critical({SRC_LOC, "Only support Int literal"});
    return -1;
  }

  auto val = code.left;
  int base = 10;
  if (val.length() > 2) {
    auto c = val[1];
    base = c == 'x' ? 16 : ('o' ? 8 : ('b' ? 2 : 10));
  }

  if (base != 10) val = val.substr(2);

  auto constant =
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(kLlvmContext), val, base);

  kName2Value[code.res] = constant;
  return 0;
}

static int GenCreate(const check::ThreeAddrCode &code) {
  if (code.op != "__create__") return 1;

  auto val = kName2Value[code.left];
  if (nullptr == val) {
    kLog->Critical({SRC_LOC, "Instruction of name", code.left, "not found"});
    return -1;
  }

  auto alloc = new llvm::AllocaInst(val->getType(), 0, code.res, kLlvmBlock);
  kName2Value[code.res] = alloc;
  new llvm::StoreInst(val, alloc, kLlvmBlock);
  return 0;
}

static int GenLoad(const check::ThreeAddrCode &code) {
  if (code.op != "__load__") return 1;

  auto src = kName2Value[code.left];
  if (nullptr == src) {
    kLog->Critical({SRC_LOC, "Instruction of name", code.left, "not found"});
    return -1;
  }

  // TODO: get value type from llvm::Value *
  auto load = new llvm::LoadInst(llvm::Type::getInt64Ty(kLlvmContext), src,
                                 code.res, kLlvmBlock);
  kName2Value[code.res] = load;
  return 0;
}

static int GenStore(const check::ThreeAddrCode &code) {
  if (code.op != "__store__") return 1;

  auto val = kName2Value[code.left];
  if (nullptr == val) {
    kLog->Critical({SRC_LOC, "Instruction of name", code.left, "not found"});
    return -1;
  }

  auto target = kName2Value[code.res];
  if (nullptr == target) {
    kLog->Critical({SRC_LOC, "Instruction of name", code.res, "not found"});
    return -1;
  }

  new llvm::StoreInst(val, target, kLlvmBlock);
  return 0;
}

static int GenLoc(const check::ThreeAddrCode &code) {
  if (code.op != "__loc__") return 1;
  if (kLlvmFunctions.empty()) {
    kLog->Critical({SRC_LOC, "loc must inside a function"});
    return -1;
  }

  auto new_block = llvm::BasicBlock::Create(kLlvmContext, code.res + code.id,
                                            kLlvmFunctions.front());
  kLoc2Block[code.id] = kLlvmBlock = new_block;
  return 0;
}

static int GenAssign(const check::ThreeAddrCode &code) {
  if (code.op != ast::OpAssign::kBuiltinName) return 1;

  auto l_val = kName2Value[code.left];
  auto target = kName2Value[code.res];
  if (l_val == nullptr || target == nullptr) {
    kLog->Critical({SRC_LOC, "l_val or target instruction not found"});
    return -1;
  }

  // TODO: Check l_val type
  new llvm::StoreInst(l_val, target, kLlvmBlock);
  return 0;
}

static int GenBop(const check::ThreeAddrCode &code) {
  decltype(llvm::Instruction::BinaryOps::Add) op = {};

  if (code.op == ast::OpPlus::kBuiltinName) {
    op = llvm::Instruction::BinaryOps::Add;
  } else if (code.op == ast::OpMinus::kBuiltinName) {
    op = llvm::Instruction::BinaryOps::Sub;
  } else if (code.op == ast::OpMul::kBuiltinName) {
    op = llvm::Instruction::BinaryOps::Mul;
  } else if (code.op == ast::OpDiv::kBuiltinName) {
    op = llvm::Instruction::BinaryOps::SDiv;
  } else {
    return 1;
  }

  auto l_val = kName2Value[code.left];
  auto r_val = kName2Value[code.right];
  if (l_val == nullptr || r_val == nullptr) {
    kLog->Critical({SRC_LOC, "l_val or r_val instruction not found"});
    return -1;
  }

  auto inst =
      llvm::BinaryOperator::Create(op, l_val, r_val, code.res, kLlvmBlock);
  kName2Value[code.res] = inst;
  return 0;
}

static int GenCmp(const check::ThreeAddrCode &code) {
  decltype(llvm::CmpInst::Predicate::ICMP_EQ) op = {};
  if (code.op == ast::OpEq::kBuiltinName) {
    op = llvm::CmpInst::Predicate::ICMP_EQ;
  } else if (code.op == ast::OpNe::kBuiltinName) {
    op = llvm::CmpInst::Predicate::ICMP_NE;
  } else if (code.op == ast::OpLe::kBuiltinName) {
    op = llvm::CmpInst::Predicate::ICMP_SLE;
  } else if (code.op == ast::OpGe::kBuiltinName) {
    op = llvm::CmpInst::Predicate::ICMP_SGE;
  } else if (code.op == ast::OpLt::kBuiltinName) {
    op = llvm::CmpInst::Predicate::ICMP_SLT;
  } else if (code.op == ast::OpGt::kBuiltinName) {
    op = llvm::CmpInst::Predicate::ICMP_SGT;
  } else {
    return 1;
  }

  auto l_val = kName2Value[code.left];
  auto r_val = kName2Value[code.right];
  if (l_val == nullptr || r_val == nullptr) {
    kLog->Critical({SRC_LOC, "l_val or r_val instruction not found"});
    return -1;
  }

  auto inst = llvm::CmpInst::Create(llvm::Instruction::ICmp, op, l_val, r_val,
                                    code.res, kLlvmBlock);
  kName2Value[code.res] = inst;
  return 0;
}

static int GenRet(const check::ThreeAddrCode &code) {
  if (code.op != "__ret__") return 1;

  auto ret_val = kName2Value[code.left];
  if (ret_val == nullptr && code.left != "") {
    kLog->Critical({SRC_LOC, "ret_val instruction not found"});
    return -1;
  }

  if (ret_val == nullptr) {
    llvm::ReturnInst::Create(kLlvmContext, kLlvmBlock);
  } else {
    llvm::ReturnInst::Create(kLlvmContext, ret_val, kLlvmBlock);
  }
  return 0;
}

static int GenJmp(const check::ThreeAddrCode &code,
                  const check::ThreeAddrCode &next_code,
                  llvm::BasicBlock *llvm_block) {
  if (code.op != "__jp__" && code.op != "__jf__" && code.op != "__jt__")
    return 1;
  if (next_code.op != "__loc__") {
    kLog->Critical({SRC_LOC, "__jmp__ must flowered by __loc__"});
    return -1;
  }

  auto jt = kLoc2Block[code.res];
  if (jt == nullptr) {
    kLog->Critical({SRC_LOC, "l_val or r_val instruction not found"});
    return -1;
  }

  if (code.op == "__jp__") {
    llvm::BranchInst::Create(jt, llvm_block);
    return 0;
  }

  auto cond = kName2Value[code.left];
  auto jf = kLoc2Block[next_code.id];
  if (cond == nullptr || jf == nullptr) {
    kLog->Critical({SRC_LOC, "cond or jf instruction not found", code.id});
    return -1;
  }

  if (code.op == "__jf__") std::swap(jt, jf);
  llvm::BranchInst::Create(jt, jf, cond, llvm_block);

  return 0;
}

static int GenCode(const check::ThreeAddrCode &code) {
  for (auto &func :
       std::initializer_list<int (*)(const check::ThreeAddrCode &)>{
           GenLoc, GenLoad, GenStore, GenBop, GenAssign, GenCmp, GenCreate,
           GenName, GenLiteral, GenRet}) {
    if (auto res = func(code); res <= 0) return res;
  }

  return 1;
}

static int GenFunc(std::forward_list<check::ThreeAddrCode>::const_iterator &it,
                   bool export_c) {
  auto name = it->res;
  if (export_c) name = name.substr(0, name.find('@'));

  if (it->op != "__begfunc__" || (++it)->op != "__type__") {
    kLog->Critical({SRC_LOC, "function must begin with __begfunc__ __type__"});
    return -1;
  }

  // llvm::Type *ret_type = kName2Value[it->left];
  // if (ret_type == nullptr) {
  //  kLog->Critical({SRC_LOC, "function return type not found"});
  //  return -1;
  //}
  // TODO: function ret_type and args
  auto ret_type = llvm::Type::getInt64Ty(kLlvmContext);
  std::vector<std::forward_list<check::ThreeAddrCode>::const_iterator>
      args_code;
  std::vector<llvm::Type *> args_type;
  ;

  while (it->op != "__begfunc_body__") {
    if (it->op == "__arg__") {
      args_type.push_back(ret_type);
      args_code.push_back(it);
    }
    ++it;
  }

  auto ftype = llvm::FunctionType::get(ret_type, args_type, false);
  auto func = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage,
                                     name, kLlvmModule);
  kLlvmFunctions.push_front(func);
  kLlvmBlock =
      llvm::BasicBlock::Create(kLlvmContext, name, kLlvmFunctions.front());
  if (name.substr(0, name.find_last_of(".")) == "__main__@") kLlvmMain = func;

  auto arg_it = func->arg_begin();
  for (int i = 0; i < args_code.size(); ++i) {
    const auto &code = *args_code[i];
    const auto &type = args_type[i];

    auto alloc = new llvm::AllocaInst(type, 0, code.res, kLlvmBlock);
    kName2Value[code.res] = alloc;
    new llvm::StoreInst(&(*arg_it++), alloc, kLlvmBlock);
  }

  for (auto next = ++it; it->op != "__endfunc__"; it = next) {
    ++next;
    auto res = GenCode(*it);
    if (res < 0) {
      kLog->Critical({"Failed to generate code", it->id});
      return res;
    }
    if (res == 0) continue;

    // is a __jmp__ code
    kJmpCodes.push_back({*it, *next, kLlvmBlock});
  }

  kJmpCodes.remove_if([](decltype(kJmpCodes.front()) &code) -> bool {
    return GenJmp(std::get<0>(code), std::get<1>(code), std::get<2>(code)) == 0;
  });

  if (kJmpCodes.size() != 0) {
    kLog->Critical(
        {"Failed to generate code", std::get<0>(kJmpCodes.front()).id});
    return -1;
  }

  llvm::ReturnInst::Create(
      kLlvmContext, llvm::ConstantInt::get(ret_type, 0, true), kLlvmBlock);
  return 0;
}

std::list<std::pair<llvm::Module *, llvm::Function *>> CodeGen(
    const std::forward_list<check::ThreeAddrCode> &codes, bool export_c) {
  std::list<std::pair<llvm::Module *, llvm::Function *>> res;
  auto it = codes.cbegin();

  if (it->op != "__begmodule__") {
    kLog->Critical({SRC_LOC, "module must begin with __begmodule__"});
    return res;
  }

  for (; it->op != "__endmodule__"; ++it) {
    kLlvmModule = new llvm::Module(it->res, kLlvmContext);
    kLlvmMain = nullptr;
    if (GenFunc(++it, export_c) != 0) return res;
    res.push_back({kLlvmModule, kLlvmMain});
  }
  return res;
}

void CodePrint(
    const std::list<std::pair<llvm::Module *, llvm::Function *>> &modules) {
  std::string res;
  auto ss = llvm::raw_string_ostream(res);

  for (const auto &pair : modules) {
    pair.first->print(ss, nullptr);
    std::cout << res << std::endl;
    res = "";
  }
}

}  // namespace codegen

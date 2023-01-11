#ifndef _XULANG_SRC_CODEGEN_CODEGEN_HPP
#define _XULANG_SRC_CODEGEN_CODEGEN_HPP

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include <list>

#include "../check/context.hpp"

namespace codegen {

std::list<std::pair<llvm::Module *, llvm::Function *>> CodeGen(
    const std::forward_list<check::ThreeAddrCode> &codes, bool export_c);

void CodePrint(const std::list<std::pair<llvm::Module *, llvm::Function *>> &);
void CodeRun(const std::list<std::pair<llvm::Module *, llvm::Function *>> &);

}  // namespace codegen

#endif  // _XULANG_SRC_CODEGEN_CODEGEN_HPP

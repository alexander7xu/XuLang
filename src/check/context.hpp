#ifndef _XULANG_SRC_CHECK_CONTEXT_HPP
#define _XULANG_SRC_CHECK_CONTEXT_HPP

#include <forward_list>
#include <map>
#include <unordered_map>

#include "../utils/log.hpp"
#include "../utils/utils.hpp"

namespace check {

extern utils::Sptr<utils::Logger> kLog;

class TypeSymbol;

namespace builtin_type_symbol {
extern utils::Uptr<TypeSymbol> kBase;
extern utils::Uptr<TypeSymbol> kFunction;
extern utils::Uptr<TypeSymbol> kStruct;
extern utils::Uptr<TypeSymbol> kClass;

extern utils::Sptr<TypeSymbol> kVoid;
extern utils::Sptr<TypeSymbol> kInt;
extern utils::Sptr<TypeSymbol> kFloat;
extern utils::Sptr<TypeSymbol> kString;
}  // namespace builtin_type_symbol

class Symbol {
  friend class Context;
  std::string _block_name;

 public:
  virtual ~Symbol() = default;
  virtual const std::string &GetName() const = 0;
  virtual TypeSymbol &GetType() const = 0;
  const std::string &GetBlockName() const { return _block_name; }
};

class TypeSymbol final : public Symbol {
 private:
  std::string _name;
  TypeSymbol *_type = nullptr;
  TypeSymbol() = default;
  // std::unordered_map<std::string, utils::Uptr<Symbol>> _attrs;

 public:
  virtual const std::string &GetName() const override { return _name; }
  virtual TypeSymbol &GetType() const override {
    if (_type == nullptr) {
      kLog->Critical({SRC_LOC, "Don't GetType() of an inbuilt type"});
      throw;
    }
    return *_type;
  }

  // type:
  //    0     :Struct
  //    1     :Class
  //    2     :Inbuilt
  //    -1    :VOID
  static utils::Uptr<TypeSymbol> New(const std::string &name, int type) {
    // _type could only be nullptr if you are creating an inbuilt type
    auto res = utils::Uptr<TypeSymbol>(new TypeSymbol());
    res->_name = name;
    if (type == 0) {
      res->_type = builtin_type_symbol::kStruct.get();
    } else if (type == 1) {
      res->_type = builtin_type_symbol::kClass.get();
    } else if (type == 2) {
      res->_type = builtin_type_symbol::kBase.get();
    } else {
      res->_type = nullptr;
    }

    if (res->_type == nullptr) {
      kLog->Info({"Adding new base type symbol:", name});
    }
    return res;
  }
};

class ObjectSymbol final : public Symbol {
  std::string _name;
  TypeSymbol &_type;
  ObjectSymbol(TypeSymbol &type) : _type(type) {}

 public:
  virtual const std::string &GetName() const override { return _name; }
  virtual TypeSymbol &GetType() const override { return _type; }

  static utils::Uptr<ObjectSymbol> New(const std::string &name,
                                       TypeSymbol &type) {
    auto res = utils::Uptr<ObjectSymbol>(new ObjectSymbol(type));
    res->_name = name;
    return res;
  }
};

class FunctionSymbol final : public Symbol {
  std::string _name;
  TypeSymbol &_type = *builtin_type_symbol::kFunction;
  TypeSymbol &_ret_type;
  FunctionSymbol(TypeSymbol &ret_type) : _ret_type(ret_type) {}

 public:
  virtual const std::string &GetName() const override { return _name; }
  virtual TypeSymbol &GetType() const override { return _type; }

  TypeSymbol &GetRetType() { return _ret_type; }
  static utils::Uptr<FunctionSymbol> New(const std::string &name,
                                         TypeSymbol &ret_type) {
    auto res = utils::Uptr<FunctionSymbol>(new FunctionSymbol(ret_type));
    res->_name = name;
    return res;
  }
};

struct ThreeAddrCode {
  std::string id;
  std::string op;
  std::string left;
  std::string right;
  std::string res;
};

class Context {
  struct Block {
    std::forward_list<Block> children;

    std::unordered_map<std::string, utils::Sptr<Symbol>> symbol_map;
    Block *parent = nullptr;
    int64_t id = 0;
    int64_t tmp_num = 0;
    int64_t children_cnt = 0;
  } _block, *_cur_block = &_block;

  std::forward_list<ThreeAddrCode> _three_addr_codes;
  int64_t _three_addr_code_cnt = 0;

 public:
  Context() {
    AddSymbol(builtin_type_symbol::kVoid);
    AddSymbol(builtin_type_symbol::kInt);
    AddSymbol(builtin_type_symbol::kFloat);
    AddSymbol(builtin_type_symbol::kString);
  }

  Block &GetCurrentBlock() { return *_cur_block; }

  std::string GetBlockId() const {
    std::string res;
    for (auto it = _cur_block; it != nullptr; it = it->parent) {
      res = std::to_string(it->id) + "." + res;
    }
    return res.substr(1);
  }

  void PushBlock() {
    _cur_block->children.push_front(
        Block{.parent = _cur_block, .id = ++_cur_block->children_cnt});
    _cur_block = &_cur_block->children.front();
  }

  void PopBlock() {
    if (_cur_block->parent == nullptr) {
      kLog->Critical({SRC_LOC, "Could not pop the head block"});
      throw;
    }
    _cur_block = _cur_block->parent;
  }

  Symbol &AddSymbol(const utils::Sptr<Symbol> &symbol) const {
    if (symbol == nullptr) {
      kLog->Critical({SRC_LOC, "Adding a null symbol is not allowed"});
      throw;
    }

    auto &name = symbol->GetName();
    auto block_name = '@' + GetBlockId();
    block_name.pop_back();
    symbol->_block_name = block_name;
    _cur_block->symbol_map[name] = std::move(symbol);
    return *_cur_block->symbol_map[name];
  }

  Symbol *FindSymbol(const std::string &name) const {
    for (auto it = _cur_block; it != nullptr; it = it->parent) {
      auto p = it->symbol_map.find(name);
      if (p != nullptr) return p->second.get();
    }
    return nullptr;
  }

  auto &GetThreeAddressCodes() const { return _three_addr_codes; }

  ThreeAddrCode &AddThreeAddressCode(const std::string &op,
                                     const std::string &left,
                                     const std::string &right,
                                     std::string res = "") {
    auto bid = GetBlockId();
    if (res == "") {
      bid.back() = '$';
      res = bid + std::to_string(++_cur_block->tmp_num);
    }

    auto id = "#" + std::to_string(++_three_addr_code_cnt);
    _three_addr_codes.push_front(ThreeAddrCode{
        .id = id, .op = op, .left = left, .right = right, .res = res});
    return _three_addr_codes.front();
  }
};

}  // namespace check

#endif  // _XULANG_SRC_CHECK_CONTEXT_HPP

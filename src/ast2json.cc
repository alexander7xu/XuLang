#include <iostream>
#include <stack>

#include "./ast/statement.hpp"
#include "./utils/log.hpp"

extern int yyparse();
extern FILE *yyin;
extern std::stack<std::string> kFilenames;
extern utils::Uptr<ast::Module> kModule;

using namespace ast;

class ToJson final : private VisitorInterface {
 private:
  std::string _result;

  template <class T>
  std::string JsonPair(const std::string &key, const Uptr<T> &val) {
    if (val == nullptr) return std::string("\"") + key + "\":\"NULL\"";
    return JsonPair(key, *val);
  }

  std::string JsonPair(const std::string &key, const ast::VargOperator &cop) {
    return JsonPair(key, dynamic_cast<const ast::Node &>(cop));
  }

  std::string JsonPair(const std::string &key, const ast::Operator &val) {
    return std::string("\"") + key + "\":\"" + val.GetName() + "\"";
  }

  std::string JsonPair(const std::string &key, const ast::Node &val) {
    auto res = std::string("\"") + key + "\":";
    const_cast<ast::Node &>(val).Accept(this);
    auto &child = this->_result;
    res += (child[0] == '{' || child[0] == '[') ? child : ('"' + child + '"');
    return res;
  }

  std::string JsonPair(const std::string &key, const std::string &val) {
    auto res = std::string("\"") + key + "\":";
    res += (val[0] == '{' || val[0] == '[') ? val : ('"' + val + '"');
    return res;
  }

  std::string JsonPair(const std::string &key, const bool &val) {
    return std::string("\"") + key + "\":\"" + (val ? "True" : "False") + "\"";
  }

 public:
  std::string operator()(const Node &node) {
    const_cast<Node &>(node).Accept(this);
    return _result;
  }

 private:
  virtual void Visit(Module *module) override {
    auto objs = std::string("[");
    for (const auto &obj : module->objs) {
      obj->Accept(this);
      objs += this->_result + ",";
    }

    if (objs.length() > 1) {
      objs[objs.length() - 1] = ']';
    } else {
      objs += "]";
    }

    auto res = std::string("{");
    res += JsonPair("class", "Module") + ",";
    res += JsonPair("filename", module->filename) + ",";
    res += JsonPair("objs", objs);
    this->_result = res += "}";
  }

  virtual void Visit(Block *block) override {
    auto stmts = std::string("[");
    for (const auto &stmt : block->statements) {
      stmt->Accept(this);
      stmts += this->_result + ",";
    }

    if (stmts.length() > 1) {
      stmts[stmts.length() - 1] = ']';
    } else {
      stmts += "]";
    }

    auto res = std::string("{");
    res += JsonPair("class", "Block") + ",";
    res += JsonPair("statements", stmts);
    this->_result = res += "}";
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
    res += JsonPair("class", "Try") + ",";
    res += JsonPair("body", try_stmt->body) + ",";
    res += JsonPair("excepts", excepts) + ",";
    res += JsonPair("orelse", try_stmt->orelse);
    _result = res += "}";
  }

  virtual void Visit(CallOperator *cop) override {
    auto unamed = std::string("[");
    for (const auto &x : cop->unameds) {
      x->Accept(this);
      unamed += this->_result + ",";
    }
    if (unamed.length() > 1) {
      unamed[unamed.length() - 1] = ']';
    } else {
      unamed += "]";
    }

    auto type_args = std::string("{");
    for (const auto &x : cop->type_args) {
      type_args += JsonPair(*std::get<0>(x), std::get<1>(x)) + ",";
    }
    if (type_args.length() > 1) {
      type_args[type_args.length() - 1] = '}';
    } else {
      type_args += "}";
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
    res += JsonPair("class", "CallOperator") + ",";
    res += JsonPair("name", std::string(cop->GetName())) + ",";
    res += JsonPair("unamed", unamed) + ",";
    res += JsonPair("type_args", type_args) + ",";
    res += JsonPair("keywords", named);
    _result = res += "}";
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
    if (dims.length() > 1) {
      dims[dims.length() - 1] = ']';
    } else {
      dims += "]";
    }

    auto res = std::string("{");
    res += JsonPair("class", "SubscriptOperator") + ",";
    res += JsonPair("dims", dims);
    _result = res += "}";
  }

  virtual void Visit(Literal *literal) override {
    auto res = std::string("{");
    res += JsonPair("class", "Literal") + ",";
    res += JsonPair("val", literal->val) + ",";
    res += JsonPair("type", literal->kTypeNames[literal->type]);
    _result = res += "}";
  }

#define _ADD_JSON_PAIR(name) (res += JsonPair(#name, leaf->name) + ",");
#define _NODE_TO_JSON_FUNC(LeafT, ...)                   \
  virtual void Visit(LeafT *leaf) override {             \
    auto res = std::string("{");                         \
    res += JsonPair("class", std::string(#LeafT)) + ","; \
    FOR_EACH(_ADD_JSON_PAIR, __VA_ARGS__);               \
    _result = res.substr(0, res.length() - 1) + "}";     \
  }

  _NODE_TO_JSON_FUNC(ExprStatement, expr)
  _NODE_TO_JSON_FUNC(Break)
  _NODE_TO_JSON_FUNC(Continue)
  _NODE_TO_JSON_FUNC(Return, expr)
  _NODE_TO_JSON_FUNC(If, test, body, orelse)
  _NODE_TO_JSON_FUNC(While, test, body, orelse)
  _NODE_TO_JSON_FUNC(ObjCreate, id, expr)
  _NODE_TO_JSON_FUNC(Function, id, args, body)
  _NODE_TO_JSON_FUNC(Assemble, id, args, body)
  _NODE_TO_JSON_FUNC(Struct, id, body)
  _NODE_TO_JSON_FUNC(Class, id, parents, body)
  _NODE_TO_JSON_FUNC(Import, id, module_root, files)
  _NODE_TO_JSON_FUNC(Raise, error)

  _NODE_TO_JSON_FUNC(Name, id, deref, parent)
  _NODE_TO_JSON_FUNC(UnaryOpExpr, op, right)
  _NODE_TO_JSON_FUNC(BinaryOpExpr, op, left, right)
  _NODE_TO_JSON_FUNC(AssignOpExpr, op, target, expr)
  _NODE_TO_JSON_FUNC(LogicExpr, op, left, right)
  _NODE_TO_JSON_FUNC(IfElseExpr, test, left, right)
  _NODE_TO_JSON_FUNC(CallExpr, obj, op)
  _NODE_TO_JSON_FUNC(SubscriptExpr, obj, op)
};

void PrintJson(const std::string &str, int depth = 0) {
  bool one_line = false, in_string = false;
  for (int i = 0; i < static_cast<int>(str.length());) {
    if (str[i] == '{' || str[i] == '[') {
      int right = str.find(str[i] == '{' ? '}' : ']', i);
      one_line = str.find(str[i], i + 1) > static_cast<size_t>(right) &&
                 str.find(',', i) > static_cast<size_t>(right);

      std::cout << str[i++];
      if (!one_line) std::cout << "\n" << std::string(++depth * 2, ' ');
    } else if (str[i] == '}' || str[i] == ']') {
      if (!one_line) std::cout << "\n" << std::string(--depth * 2, ' ');
      std::cout << str[i++];

      if (str[i] == ',') {
        std::cout << str[i++] << '\n' << std::string(depth * 2, ' ');
        i += str[i] == ' ';
      } else if (str[i] == '}') {
        one_line = false;
      }
    } else {
      std::cout << str[i];
      in_string ^= str[i] == '"';
      if (str[i++] == ',' && !in_string) {
        std::cout << '\n' << std::string(depth * 2, ' ');
        i += str[i] == ' ';
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: parser file1.xl file2.xl file3.xl ..." << std::endl;
    return 0;
  }

  for (int i = 1; i < argc; ++i) {
    kFilenames.push(argv[i]);
    yyin = fopen(argv[i], "r+");
    if (yyparse() != 0) return -1;

    auto to_json = ToJson();
    PrintJson(to_json(*kModule));
    std::cout << std::endl << std::endl;
  }

  return 0;
}

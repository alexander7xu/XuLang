#ifndef _XULANG_SRC_AST_NODE_HPP
#define _XULANG_SRC_AST_NODE_HPP

#include <list>

#include "./visitor.hpp"

namespace ast {

using TextType = std::string;

// The base of all AST node classes
class Node {
 public:
  virtual ~Node() = default;
  virtual void Accept(VisitorInterface *visitor) = 0;
};

}  // namespace ast

#endif  // _XULANG_SRC_AST_NODE_HPP

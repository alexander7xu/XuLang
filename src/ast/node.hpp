#ifndef _XULANG_SRC_AST_NODE_HPP
#define _XULANG_SRC_AST_NODE_HPP

#include <list>

#include "./visitor.hpp"

namespace ast {

using TextType = std::string;

struct SourceCodeLocator {
  int line_beg;
  int col_beg;
  int line_end;
  int col_end;
  int file_idx;

  operator std::string() const {
    return std::string("Line ") + std::to_string(line_beg) + " Col " +
           std::to_string(col_beg);
  }
};

// The base of all AST node classes
class Node {
 public:
  virtual ~Node() = default;
  virtual void Accept(VisitorInterface *visitor) = 0;
};

}  // namespace ast

#endif  // _XULANG_SRC_AST_NODE_HPP

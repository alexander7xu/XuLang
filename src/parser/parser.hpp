#ifndef _XULANG_SRC_PARSER_PARSER_HPP
#define _XULANG_SRC_PARSER_PARSER_HPP

#include <vector>

#include "../ast/statement.hpp"
#include "./lexer.hpp"

namespace parser {

class Parser final {
 public:
  bool Parse(const char *filename);
  const std::vector<std::string> &GetFilenames();
  utils::Sptr<ast::Module> GetAst();
  utils::Sptr<parser::Lexer> GetLexer();
};

}  // namespace parser

#endif

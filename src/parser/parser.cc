#include "./parser.hpp"

#include "../utils/log.hpp"

extern int yyparse();
extern FILE *yyin;

std::vector<std::string> kFilenames;
utils::Sptr<parser::Lexer> kLexer = nullptr;
utils::Sptr<ast::Module> kModule = nullptr;

namespace parser {
auto kLog = utils::Logger::New("parser");

bool Parser::Parse(const char *filename) {
  kLexer = utils::Sptr<parser::Lexer>(new parser::Lexer());
  kModule = nullptr;

  kFilenames.push_back(filename);
  yyin = fopen(filename, "r");
  auto res = yyparse() == 0;
  fclose(yyin);
  return res;
}

const std::vector<std::string> &Parser::GetFilenames() { return kFilenames; }
utils::Sptr<ast::Module> Parser::GetAst() { return kModule; }
utils::Sptr<parser::Lexer> Parser::GetLexer() { return kLexer; }

}  // namespace parser

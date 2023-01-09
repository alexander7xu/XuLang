#ifndef _XULANG_SRC_PARSER_LEXER_HPP
#define _XULANG_SRC_PARSER_LEXER_HPP

#include <list>

#include "../utils/utils.hpp"

namespace parser {

struct SourceCodeLocator {
  int line;
  int col;
  int file_idx;

  operator std::string() const {
    return std::string("Line ") + std::to_string(line) + " Col " +
           std::to_string(col);
  }
};

class Token {
  SourceCodeLocator _loc;
  std::string _text;

 public:
  virtual ~Token() = default;
  virtual const char *GetTypeName() const = 0;
  inline const SourceCodeLocator &GetLoc() const { return _loc; }
  inline const std::string &GetText() const { return _text; }

 protected:
  Token(const SourceCodeLocator &loc, const std::string &text)
      : _loc(loc), _text(text) {}
};

class Annotation final : public Token {
 public:
  Annotation(const SourceCodeLocator &loc, const std::string &text)
      : Token(loc, text) {}
  virtual const char *GetTypeName() const override { return "Annotation"; }
};

class Trivial final : public Token {
 public:
  Trivial(const SourceCodeLocator &loc, const std::string &text)
      : Token(loc, text) {}
  virtual const char *GetTypeName() const override { return "Trivial"; }
};

class Keyword final : public Token {
 public:
  Keyword(const SourceCodeLocator &loc, const std::string &text)
      : Token(loc, text) {}
  virtual const char *GetTypeName() const override { return "Keyword"; }
};

class Operator final : public Token {
 public:
  Operator(const SourceCodeLocator &loc, const std::string &text)
      : Token(loc, text) {}
  virtual const char *GetTypeName() const override { return "Operator"; }
};

class Identifier final : public Token {
 public:
  Identifier(const SourceCodeLocator &loc, const std::string &text)
      : Token(loc, text) {}
  virtual const char *GetTypeName() const override { return "Identifier"; }
};

class Literal final : public Token {
 public:
  enum Type { kInt, kFloat, kString };

 private:
  Type _type;
  inline static const char *const kTypeNames[] = {"Int", "Float", "String"};

 public:
  Literal(const SourceCodeLocator &loc, const std::string &text, Type type)
      : Token(loc, text), _type(type) {}
  virtual const char *GetTypeName() const override { return kTypeNames[_type]; }
};

class Lexer {
 private:
  std::list<utils::Uptr<Token>> _tokens;

 public:
  inline const auto &GetTokens() const { return _tokens; }

  template <class TokenT>
  inline void AddToken(const SourceCodeLocator &loc, const std::string &text) {
    _tokens.push_back(utils::Uptr<Token>(new TokenT(loc, text)));
  }

  inline void AddToken(const SourceCodeLocator &loc, const std::string &text,
                       Literal::Type type) {
    _tokens.push_back(utils::Uptr<Token>(new Literal(loc, text, type)));
  }
};

}  // namespace parser

#endif  // _XULANG_SRC_PARSER_LEXER_HPP

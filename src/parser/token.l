%{
#include <vector>

#include "parser.hpp"
#include "utils/log.hpp"
#include "parser/parser.hpp"


#define TOKEN_LITERAL(name, type)                   \
    yylval.TextP = new std::string(yytext, yyleng); \
    kLexer->AddToken(yylloc, *yylval.TextP, type);  \
    return name;

#define TOKEN_IDENTIFIER()                                          \
    yylval.TextP = new std::string(yytext, yyleng);                 \
    kLexer->AddToken<parser::Identifier>(yylloc, *yylval.TextP);    \
    return TK_IDENTIFIER;                                           \

#define TOKEN_ANNOTATION()                                                      \
    kLexer->AddToken<parser::Annotation>(yylloc, std::string(yytext, yyleng));

#define TOKEN_OPERATOR(name)                                                    \
    yylval.token = name;                                                        \
    kLexer->AddToken<parser::Operator>(yylloc, std::string(yytext, yyleng));    \
    return name;

#define TOKEN_KEYWORD(name)                                                 \
    yylval.token = name;                                                    \
    kLexer->AddToken<parser::Keyword>(yylloc, std::string(yytext, yyleng)); \
    return name;

#define TOKEN_TRIVIAL(name)                                                 \
    yylval.token = name;                                                    \
    kLexer->AddToken<parser::Trivial>(yylloc, std::string(yytext, yyleng)); \
    return name;


static auto kLog = utils::Logger::Get("parser", true);
extern utils::Sptr<parser::Lexer> kLexer;
extern std::vector<std::string> kFilenames;

extern "C" int yywrap() { return 1; }
static int yycolumn = 1;

void yyerror(const char *s) {
    kLog->Error({"file \"" + kFilenames.back() + "\"", std::string(yylloc)+":", s});
}

#define YY_USER_ACTION                                      \
    if (auto idx = static_cast<int>(kFilenames.size()) - 1; \
            yylloc.file_idx != idx) {                       \
        yylloc.file_idx = idx;                              \
        yylineno = yycolumn = 1;                            \
    }                                                       \
    if (yylloc.line != yylineno) {                          \
        yylloc.line = yylineno;                             \
        yycolumn = 1;                                       \
    }                                                       \
    yylloc.col = yycolumn;                                  \
    yycolumn += static_cast<int>(yyleng);                   \
    yylloc.file_idx = static_cast<int>(kFilenames.size()) - 1;
%}

%option yylineno

%%
#.*             TOKEN_ANNOTATION();
[ \t\r]+        ;   // Ignored
[ ]*[\n]+[ ]*   TOKEN_TRIVIAL(TK_LF);

"break"     TOKEN_KEYWORD(TK_BREAK);
"continue"  TOKEN_KEYWORD(TK_CONTINUE);
"return"    TOKEN_KEYWORD(TK_RETURN);
"raise"     TOKEN_KEYWORD(TK_RAISE);
"if"        TOKEN_KEYWORD(TK_IF);
"else"      TOKEN_KEYWORD(TK_ELSE);
"while"     TOKEN_KEYWORD(TK_WHILE);
"Function"  TOKEN_KEYWORD(TK_FUNC);
"Assemble"  TOKEN_KEYWORD(TK_ASM);
"Struct"    TOKEN_KEYWORD(TK_STRUCT);
"Class"     TOKEN_KEYWORD(TK_CLASS);
"Import"    TOKEN_KEYWORD(TK_IMPORT);
"try"       TOKEN_KEYWORD(TK_TRY);
"except"    TOKEN_KEYWORD(TK_EXCEPT);

[a-zA-Z_][a-zA-Z0-9_]*                  TOKEN_IDENTIFIER();
[0-9]+                                  TOKEN_LITERAL(TK_INTEGER, parser::Literal::Type::kInt);
0b[0-1]+                                TOKEN_LITERAL(TK_INTEGER, parser::Literal::Type::kInt);
0o[0-7]+                                TOKEN_LITERAL(TK_INTEGER, parser::Literal::Type::kInt);
0x[0-9a-fA-F]+                          TOKEN_LITERAL(TK_INTEGER, parser::Literal::Type::kInt);
[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)?     TOKEN_LITERAL(TK_FLOAT, parser::Literal::Type::kFloat);
('(\\'|[^'])*')                         TOKEN_LITERAL(TK_STRING, parser::Literal::Type::kString);

":="    TOKEN_OPERATOR(TK_CREATE);
"="     TOKEN_OPERATOR(TK_ASSIGN);

"+"     TOKEN_OPERATOR(TK_PLUS);
"-"     TOKEN_OPERATOR(TK_MINUS);
"*"     TOKEN_OPERATOR(TK_MUL);
"/"     TOKEN_OPERATOR(TK_DIV);
"%"     TOKEN_OPERATOR(TK_MOD);

"^"     TOKEN_OPERATOR(TK_BXOR);
"|"     TOKEN_OPERATOR(TK_BOR);
"&"     TOKEN_OPERATOR(TK_BAND);
"~"     TOKEN_OPERATOR(TK_BNOT);
"<<"    TOKEN_OPERATOR(TK_SHIFT_L);
">>"    TOKEN_OPERATOR(TK_SHIFT_R);

"+="    TOKEN_OPERATOR(TK_SELF_PLUS);
"-="    TOKEN_OPERATOR(TK_SELF_MINUS);
"*="    TOKEN_OPERATOR(TK_SELF_MUL);
"/="    TOKEN_OPERATOR(TK_SELF_DIV);
"%="    TOKEN_OPERATOR(TK_SELF_MOD);

"^="    TOKEN_OPERATOR(TK_SELF_BXOR);
"|="    TOKEN_OPERATOR(TK_SELF_BOR);
"&="    TOKEN_OPERATOR(TK_SELF_BAND);
"<<="   TOKEN_OPERATOR(TK_SELF_SHIFT_L);
">>="   TOKEN_OPERATOR(TK_SELF_SHIFT_R);

"!"     TOKEN_OPERATOR(TK_NOT);
"||"    TOKEN_OPERATOR(TK_OR);
"&&"    TOKEN_OPERATOR(TK_AND);
"=="    TOKEN_OPERATOR(TK_EQ);
"!="    TOKEN_OPERATOR(TK_NE);
"<="    TOKEN_OPERATOR(TK_LE);
">="    TOKEN_OPERATOR(TK_GE);
"<"     TOKEN_OPERATOR(TK_LT);
">"     TOKEN_OPERATOR(TK_GT);
"."     TOKEN_OPERATOR(TK_MEMBER);
"->"    TOKEN_OPERATOR(TK_DEREF_MEMBER);

"("     TOKEN_TRIVIAL(TK_PAREN_L);
")"     TOKEN_TRIVIAL(TK_PAREN_R);
"["     TOKEN_TRIVIAL(TK_BRACKET_L);
"]"     TOKEN_TRIVIAL(TK_BRACKET_R);
"{"     TOKEN_TRIVIAL(TK_BRACE_L);
"}"     TOKEN_TRIVIAL(TK_BRACE_R);
","     TOKEN_TRIVIAL(TK_COMMA);
":"     TOKEN_TRIVIAL(TK_COLON);

. yyerror("Unknown token"); yyterminate();
%%

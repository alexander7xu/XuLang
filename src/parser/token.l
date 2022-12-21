%{
#include "parser.hpp"
#include "utils/log.hpp"

#include <stack>

#define SAVE_LITERAL()  (yylval.TextP = new std::string(yytext, yyleng))
#define TOKEN(t)        (yylval.token = t)

extern std::stack<std::string> kFilenames;
extern "C" int yywrap() { return 1; }

static int yycolumn = 1;
static auto kLog = utils::Logger::NewLogger("parser");

void yyerror(const char *s) {
    kLog->Error({"file \"" + kFilenames.top() + "\"", std::string(yylloc)+":", s});
}

static void LogAction() {
    std::string text;
    for (const auto &c : std::string(yytext, yyleng)) {
        if (c == '\n') text += "<LF>";
        else if (c == '\r') text += "<CR>";
        else if (c == '\t') text += "<TAB>";
        else if (c == ' ') text += "<SPACE>";
        else text += c;
    }
    auto loc = std::string(yylloc);
    kLog->Info({"File", kFilenames.top(), loc + std::string(16 - loc.size(), ' '), text});
}

#define YY_USER_ACTION                                                \
    if (yylloc.file_idx != static_cast<int>(kFilenames.size()) - 1) { \
        yylloc.file_idx = static_cast<int>(kFilenames.size()) - 1;    \
        yylineno = yycolumn = 1;                                      \
    }                                                                 \
    if (yylloc.line_end < yylineno) yycolumn = 1;                     \
    yylloc.line_beg = yylloc.line_end = yylineno;                     \
    yylloc.col_beg = yycolumn;                                        \
    yylloc.col_end = yycolumn + (int)yyleng;                          \
    yycolumn += (int)yyleng;                                          \
    yylloc.file_idx = static_cast<int>(kFilenames.size()) - 1;        \
    LogAction();
%}

%option yylineno

%%
#.*         // Comment line
[ \t\r]+ ;  // Ignored
[\n]+       yycolumn = 1; return TOKEN(TK_LF);

"break"     return TOKEN(TK_BREAK);
"continue"  return TOKEN(TK_CONTINUE);
"return"    return TOKEN(TK_RETURN);
"raise"     return TOKEN(TK_RAISE);
"if"        return TOKEN(TK_IF);
"else"      return TOKEN(TK_ELSE);
"while"     return TOKEN(TK_WHILE);
"Function"  return TOKEN(TK_FUNC);
"Assemble"  return TOKEN(TK_ASM);
"Struct"    return TOKEN(TK_STRUCT);
"Class"     return TOKEN(TK_CLASS);
"Import"    return TOKEN(TK_IMPORT);
"try"       return TOKEN(TK_TRY);
"except"    return TOKEN(TK_EXCEPT);

[a-zA-Z_][a-zA-Z0-9_]*                      SAVE_LITERAL(); return TK_IDENTIFIER;
[0-9]+                                      SAVE_LITERAL(); return TK_INTEGER;
0b[0-1]+                                    SAVE_LITERAL(); return TK_INTEGER;
0o[0-7]+                                    SAVE_LITERAL(); return TK_INTEGER;
0x[0-9a-fA-F]+                              SAVE_LITERAL(); return TK_INTEGER;
[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)?         SAVE_LITERAL(); return TK_FLOAT;
('(\\'|[^'])*')                             SAVE_LITERAL(); return TK_STRING;

":="    return TOKEN(TK_CREATE);
"="     return TOKEN(TK_ASSIGN);

"+"     return TOKEN(TK_PLUS);
"-"     return TOKEN(TK_MINUS);
"*"     return TOKEN(TK_MUL);
"/"     return TOKEN(TK_DIV);
"%"     return TOKEN(TK_MOD);

"^"     return TOKEN(TK_BXOR);
"|"     return TOKEN(TK_BOR);
"&"     return TOKEN(TK_BAND);
"~"     return TOKEN(TK_BNOT);
"<<"    return TOKEN(TK_SHIFT_L);
">>"    return TOKEN(TK_SHIFT_R);

"+="    return TOKEN(TK_SELF_PLUS);
"-="    return TOKEN(TK_SELF_MINUS);
"*="    return TOKEN(TK_SELF_MUL);
"/="    return TOKEN(TK_SELF_DIV);
"%="    return TOKEN(TK_SELF_MOD);

"^="    return TOKEN(TK_SELF_BXOR);
"|="    return TOKEN(TK_SELF_BOR);
"&="    return TOKEN(TK_SELF_BAND);
"<<="   return TOKEN(TK_SELF_SHIFT_L);
">>="   return TOKEN(TK_SELF_SHIFT_R);

"!"     return TOKEN(TK_NOT);
"||"    return TOKEN(TK_OR);
"&&"    return TOKEN(TK_AND);
"=="    return TOKEN(TK_EQ);
"!="    return TOKEN(TK_NE);
"<="    return TOKEN(TK_LE);
">="    return TOKEN(TK_GE);
"<"     return TOKEN(TK_LT);
">"     return TOKEN(TK_GT);

"("     return TOKEN(TK_PAREN_L);
")"     return TOKEN(TK_PAREN_R);
"["     return TOKEN(TK_BRACKET_L);
"]"     return TOKEN(TK_BRACKET_R);
"{"     return TOKEN(TK_BRACE_L);
"}"     return TOKEN(TK_BRACE_R);
","     return TOKEN(TK_COMMA);
":"     return TOKEN(TK_COLON);
"."     return TOKEN(TK_MEMBER);
"->"    return TOKEN(TK_DEREF_MEMBER);

. yyerror("Unknown token"); yyterminate();
%%

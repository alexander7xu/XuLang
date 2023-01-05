// Define our terminal symbols (tokens). This should match our
// tokens.l lex file. We also define the node type they represent.

%code requires {
    #include "ast/statement.hpp"

    #define MVU(p)      (std::unique_ptr<std::remove_reference<decltype(*p)>::type>(p))
    #define UNEW(exp)   (utils::Uptr<decltype(exp)>(new exp))

    #define YYLTYPE_IS_DECLARED
    #define YYLTYPE ast::SourceCodeLocator

    #define YYLLOC_DEFAULT(cur, x, n)                                  \
        if (n > 0) {                                                   \
            (cur).line_beg = YYRHSLOC(x, 1).line_beg;                  \
            (cur).col_beg = YYRHSLOC(x, 1).col_beg;                    \
            (cur).line_end = YYRHSLOC(x, n).line_end;                  \
            (cur).col_end = YYRHSLOC(x, n).col_end;                    \
            (cur).file_idx = kFilenames.size() - 1;                    \
        } else {                                                       \
            (cur).line_beg = (cur).line_end = YYRHSLOC(x, 0).line_end; \
            (cur).col_beg = (cur).col_end = YYRHSLOC(x, 0).col_end;    \
            (cur).file_idx = kFilenames.size() - 1;                    \
        }

}

%{
    #include "../src/ast/statement.hpp"
    #include <stack>
    
    int yylex();
    void yyerror(char const *s);

    std::stack<std::string> kFilenames;
    utils::Uptr<ast::Module> kModule = nullptr; // the top level root node of our final AST
%}


%union {
    ast::TextType           *TextP;
    int                     token;

    ast::Node               *NodeP;
    ast::Statement          *StatementP;
    ast::Expression         *ExpressionP;
    ast::Create             *CreateP;

    ast::Module             *ModuleP;
    ast::Block              *BlockP;
    ast::If                 *IfP;
    ast::Try                *TryP;
    ast::Name               *NameP;
    ast::CallExpr           *CallExprP;
    ast::CallOperator       *CallOperatorP;
    ast::SubscriptOperator  *SubscriptOperatorP;
    ast::SubscriptOperator::SubscriptArg *SubscriptArgP;
}


%token <token>  TK_LF
%token <token>  TK_IF TK_ELSE TK_WHILE TK_CONTINUE TK_BREAK TK_RETURN TK_RAISE TK_TRY TK_EXCEPT
%token <token>  TK_IMPORT TK_FUNC TK_ASM TK_STRUCT TK_CLASS
%token <TextP>  TK_IDENTIFIER TK_STRING TK_INTEGER TK_FLOAT
%token <token>  TK_CREATE TK_ASSIGN
%token <token>  TK_PLUS TK_MINUS TK_MUL TK_DIV TK_MOD
%token <token>  TK_BXOR TK_BOR TK_BAND TK_BNOT TK_SHIFT_L TK_SHIFT_R
%token <token>  TK_SELF_PLUS TK_SELF_MINUS TK_SELF_MUL TK_SELF_DIV TK_SELF_MOD
%token <token>  TK_SELF_BXOR TK_SELF_BOR TK_SELF_BAND TK_SELF_SHIFT_L TK_SELF_SHIFT_R
%token <token>  TK_NOT TK_OR TK_AND TK_EQ TK_NE TK_LE TK_GE TK_LT TK_GT
%token <token>  TK_PAREN_L TK_PAREN_R TK_BRACKET_L TK_BRACKET_R TK_BRACE_L TK_BRACE_R
%token <token>  TK_COMMA TK_COLON TK_MEMBER TK_DEREF_MEMBER


%type <StatementP>  stmt
%type <StatementP>  break continue return raise
%type <StatementP>  if while try
%type <ExpressionP> expr
%type <ExpressionP> literal uop_expr bop_expr logic_expr
%type <ExpressionP> subscript if_else
%type <CreateP>     create obj_create function assemble struct class import

%type <ModuleP>             module
%type <BlockP>              block _stmts
%type <IfP>                 _beg_if
%type <TryP>                _beg_try
%type <NameP>               name
%type <CallExprP>           call
%type <CallOperatorP>       op_call _unamed_args _type_args _named_args
%type <SubscriptOperatorP>  op_subscript _subscript_list
%type <SubscriptArgP>       _subscript_arg


// https://en.cppreference.com/w/cpp/language/operator_precedence
%right  TK_IF TK_ELSE TK_ASSIGN TK_SELF_PLUS TK_SELF_MINUS TK_SELF_MUL TK_SELF_DIV TK_SELF_MOD TK_SELF_BXOR TK_SELF_BOR TK_SELF_BAND TK_SELF_SHIFT_L TK_SELF_SHIFT_R
%left   TK_OR
%left   TK_AND
%left   TK_BOR
%left   TK_BXOR
%left   TK_BAND
%left   TK_EQ TK_NE
%left   TK_LT TK_LE TK_GT TK_GE
%left   TK_SHIFT_L TK_SHIFT_R
%left   TK_PLUS TK_MINUS
%left   TK_MUL TK_DIV TK_MOD
%precedence  PR_UOP
%precedence  TK_PAREN_L TK_BRACKET_L TK_MEMBER TK_DEREF_MEMBER

%start start
%locations

%%
start   : module { kModule = MVU($1); }
        ;
module  : module TK_LF create { $1->AddObj(MVU($3)); }
        | module TK_LF { $$ = $1; }
        | create { $$ = new ast::Module(kFilenames.top(), MVU($1)); }
        | %empty { $$ = new ast::Module(kFilenames.top()); }
        ;
create  : obj_create | function | assemble | struct | class | import
        ;
block   : TK_BRACE_L _stmts TK_BRACE_R { $$ = $2; }
        ;
_stmts  : _stmts TK_LF stmt { $1->AddStatement(MVU($3)); }
        | _stmts TK_LF { $$ = $1; }
        | stmt { $$ = new ast::Block(MVU($1));}
        | %empty { $$ = new ast::Block(); }
        ;

stmt        : create { $$ = $1; }
            | break
            | continue
            | return
            | raise
            | if
            | while
            | try
            | expr { $$ = new ast::ExprStatement(MVU($1)); }
            ;
obj_create  : TK_IDENTIFIER TK_CREATE expr { $$ = new ast::ObjCreate(MVU($1), MVU($3)); }
            ;
break       : TK_BREAK { $$ = new ast::Break(); }
            ;
continue    : TK_CONTINUE { $$ = new ast::Continue(); }
            ;
return      : TK_RETURN expr { $$ = new ast::Return(MVU($2)); }
            | TK_RETURN { $$ = new ast::Return(nullptr); }
            ;
raise       : TK_RAISE expr { $$ = new ast::Raise(MVU($2)); }
            ;
if          : _beg_if TK_ELSE block { $$ = $1; $1->SetOrelse(MVU($3)); }
            | _beg_if { $$ = $1; }
            ;
_beg_if     : TK_IF TK_PAREN_L expr TK_PAREN_R block { $$ = new ast::If(MVU($3), MVU($5)); }
            | _beg_if TK_ELSE TK_IF TK_PAREN_L expr TK_PAREN_R block
                { $$ = $1; $1->SetOrelse(UNEW(ast::Block( UNEW(ast::If(MVU($5), MVU($7))) ))); }
            ;
while       : TK_WHILE TK_PAREN_L expr TK_PAREN_R block TK_ELSE block
                { $$ = new ast::While(MVU($3), MVU($5), MVU($7)); }
            | TK_WHILE TK_PAREN_L expr TK_PAREN_R block
                { $$ = new ast::While(MVU($3), MVU($5)); }
            ;
function    : TK_IDENTIFIER TK_CREATE TK_FUNC op_call block
                { $$ = new ast::Function(MVU($1), MVU($4), MVU($5)); }
            ;
assemble    : TK_IDENTIFIER TK_CREATE TK_ASM op_call block
                { $$ = new ast::Assemble(MVU($1), MVU($4), MVU($5)); }
            ;
struct      : TK_IDENTIFIER TK_CREATE TK_STRUCT TK_PAREN_L TK_PAREN_R block
                { $$ = new ast::Struct(MVU($1), MVU($6)); }
            ;
class       : TK_IDENTIFIER TK_CREATE TK_CLASS TK_PAREN_L _unamed_args TK_PAREN_R block
                { $$ = new ast::Class(MVU($1), MVU($5), MVU($7)); }
            ;
import      : TK_IDENTIFIER TK_CREATE TK_IMPORT TK_PAREN_L _unamed_args TK_PAREN_R block
                { $$ = new ast::Import(MVU($1), MVU($5), MVU($7)); }
            ;
try         : _beg_try TK_ELSE block { $$ = $1; $1->SetOrelse(MVU($3)); }
            | _beg_try { $$ = $1; }
            ;
_beg_try    : TK_TRY block TK_EXCEPT TK_PAREN_L TK_IDENTIFIER TK_CREATE name TK_PAREN_R block
                { $$ = new ast::Try(MVU($2)); $$->AddExcept({MVU($5), MVU($7), MVU($9)}); }
            | _beg_try TK_EXCEPT TK_PAREN_L TK_IDENTIFIER TK_CREATE name TK_PAREN_R block
                { $$->AddExcept({MVU($4), MVU($6), MVU($8)}); }
            ;

expr        : literal
            | name  { $$ = $1; }
            | uop_expr
            | bop_expr
            | logic_expr
            | call { $$ = $1; }
            | subscript
            | if_else
            | TK_PAREN_L expr TK_PAREN_R { $$ = $2; }
            ;
name        : expr TK_MEMBER TK_IDENTIFIER
                { $$ = new ast::Name(MVU($3), false, MVU($1)); }
            | expr TK_DEREF_MEMBER TK_IDENTIFIER
                { $$ = new ast::Name(MVU($3), true, MVU($1)); }
            | TK_IDENTIFIER { $$ = new ast::Name(MVU($1)); }
            ;
literal     : TK_INTEGER { $$ = new ast::Literal(MVU($1), builtin::kInt); }
            | TK_FLOAT { $$ = new ast::Literal(MVU($1), builtin::kFloat); }
            | TK_STRING { $$ = new ast::Literal(MVU($1), builtin::kString); }
            ;
call        : expr op_call { $$ = new ast::CallExpr(MVU($1), MVU($2)); }
            ;
subscript   : expr op_subscript { $$ = new ast::SubscriptExpr(MVU($1), MVU($2)); }
            ;
if_else     : expr TK_IF expr TK_ELSE expr
                { $$ = new ast::IfElseExpr(MVU($1), MVU($3), MVU($5)); }
            ;

op_call     : TK_PAREN_L _named_args TK_PAREN_R { $$ = $2; }
            ;
_named_args : _named_args TK_COMMA TK_IDENTIFIER TK_CREATE expr
                { $1->AddKeyword(MVU($3), MVU($5)); }
            | _type_args { $$ = $1; }
            ;
_type_args  : _type_args TK_COMMA TK_IDENTIFIER TK_COLON name
                { $1->AddTypeArg(MVU($3), MVU($5)); }
            | _unamed_args { $$ = $1; }
            ;
_unamed_args: _unamed_args TK_COMMA expr { $1->AddUnamed(MVU($3)); }
            | expr { $$ = new ast::CallOperator(); $$->AddUnamed(MVU($1)); }
            | %empty { $$ = new ast::CallOperator(); }
            ;

op_subscript    : TK_BRACKET_L _subscript_list TK_BRACKET_R { $$ = $2; }
                ;
_subscript_list : _subscript_list TK_COMMA _subscript_arg { $1->AddDim(MVU($3)); }
                | _subscript_arg { $$ = new ast::SubscriptOperator(); $$->AddDim(MVU($1)); }
                ;
_subscript_arg  : expr TK_COLON expr TK_COLON expr
                    { $$ = new ast::SubscriptOperator::SubscriptArg({MVU($1), MVU($3), MVU($5)}); }
                | TK_COLON expr TK_COLON expr
                    { $$ = new ast::SubscriptOperator::SubscriptArg({nullptr, MVU($2), MVU($4)}); }
                | expr TK_COLON TK_COLON expr
                    { $$ = new ast::SubscriptOperator::SubscriptArg({MVU($1), nullptr, MVU($4)}); }
                | expr TK_COLON expr _colon
                    { $$ = new ast::SubscriptOperator::SubscriptArg({MVU($1), MVU($3), nullptr}); }
                | expr _colon_pair
                    { $$ = new ast::SubscriptOperator::SubscriptArg({MVU($1), nullptr, nullptr}); }
                | TK_COLON expr _colon
                    { $$ = new ast::SubscriptOperator::SubscriptArg({nullptr, MVU($2), nullptr}); }
                | TK_COLON TK_COLON expr
                    { $$ = new ast::SubscriptOperator::SubscriptArg({nullptr, nullptr, MVU($3)}); }
                ;
_colon_pair     : TK_COLON TK_COLON
                | TK_COLON
                | %empty
                ;
_colon          : TK_COLON
                | %empty
                ;

uop_expr    : TK_BNOT expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpBitNot()), MVU($2)); }
            | TK_NOT expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpNot()), MVU($2)); }
            | TK_PLUS expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpPositive()), MVU($2)); }
            | TK_MINUS expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpNegative()), MVU($2)); }
            | TK_MUL expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpDeref()), MVU($2)); }
            | TK_BAND expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpRef()), MVU($2)); }
            ;

bop_expr    : expr TK_PLUS expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpPlus()), MVU($3)); }
            | expr TK_MINUS expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpMinus()), MVU($3)); }
            | expr TK_MUL expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpMul()), MVU($3)); }
            | expr TK_DIV expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpMod()), MVU($3)); }
            | expr TK_MOD expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpDiv()), MVU($3)); }
            | expr TK_BXOR expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpBitXor()), MVU($3)); }
            | expr TK_BOR expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpBitOr()), MVU($3)); }
            | expr TK_BAND expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpBitAnd()), MVU($3)); }
            | expr TK_SHIFT_L expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpShiftL()), MVU($3)); }
            | expr TK_SHIFT_R expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpShiftR()), MVU($3)); }

            | expr TK_ASSIGN expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpAssign()), MVU($3)); }
            | expr TK_SELF_PLUS expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfPlus()), MVU($3)); }
            | expr TK_SELF_MINUS expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfMinus()), MVU($3)); }
            | expr TK_SELF_MUL expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfMul()), MVU($3)); }
            | expr TK_SELF_DIV expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfMod()), MVU($3)); }
            | expr TK_SELF_MOD expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfDiv()), MVU($3)); }
            | expr TK_SELF_BXOR expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfBitXor()), MVU($3)); }
            | expr TK_SELF_BOR expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfBitOr()), MVU($3)); }
            | expr TK_SELF_BAND expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfBitAnd()), MVU($3)); }
            | expr TK_SELF_SHIFT_L expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfShiftL()), MVU($3)); }
            | expr TK_SELF_SHIFT_R expr { $$ = new ast::BinaryOpExpr(MVU($1), UNEW(ast::OpSelfShiftR()), MVU($3)); }
            ;

logic_expr  : expr TK_OR expr { $$ = new ast::LogicExpr(MVU($1), UNEW(ast::OpOr()), MVU($3)); }
            | expr TK_AND expr { $$ = new ast::LogicExpr(MVU($1), UNEW(ast::OpAnd()), MVU($3)); }
            | expr TK_EQ expr { $$ = new ast::LogicExpr(MVU($1), UNEW(ast::OpEq()), MVU($3)); }
            | expr TK_NE expr { $$ = new ast::LogicExpr(MVU($1), UNEW(ast::OpNe()), MVU($3)); }
            | expr TK_LE expr { $$ = new ast::LogicExpr(MVU($1), UNEW(ast::OpLe()), MVU($3)); }
            | expr TK_GE expr { $$ = new ast::LogicExpr(MVU($1), UNEW(ast::OpGe()), MVU($3)); }
            | expr TK_LT expr { $$ = new ast::LogicExpr(MVU($1), UNEW(ast::OpLt()), MVU($3)); }
            | expr TK_GT expr { $$ = new ast::LogicExpr(MVU($1), UNEW(ast::OpGt()), MVU($3)); }
            ;
%%

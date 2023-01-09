// Define our terminal symbols (tokens). This should match our
// tokens.l lex file. We also define the node type they represent.

%code requires {
    #include "parser/parser.hpp"

    #define B2U(p)      (utils::Uptr<std::remove_reference<decltype(*p)>::type>(p))
    #define UNEW(exp)   (utils::Uptr<decltype(exp)>(new exp))

    #define YYLTYPE_IS_DECLARED
    #define YYLTYPE parser::SourceCodeLocator

    #define YYLLOC_DEFAULT(cur, x, n)               \
            (cur).line = YYRHSLOC(x, 0).line;       \
            (cur).col = YYRHSLOC(x, 0).col;         \
            (cur).file_idx = kFilenames.size() - 1;
}

%{
    #include "parser/parser.hpp"
    #include <vector>
    
    int yylex();
    void yyerror(char const *s);

    extern std::vector<std::string> kFilenames;
    extern utils::Sptr<ast::Module> kModule; // the top level root node of our final AST
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
    ast::LogicExpr          *LogicExprP;
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
%type <StatementP>  if while try _end_if
%type <ExpressionP> expr
%type <ExpressionP> literal uop_expr bop_expr aop_expr
%type <LogicExprP>  logic_expr
%type <ExpressionP> subscript if_else
%type <CreateP>     create obj_create function assemble struct class import

%type <ModuleP>             module
%type <BlockP>              block _stmts
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
_lf         : TK_LF | %empty
            ;
_comma      : TK_COMMA _lf
            ;
_paren_l    : TK_PAREN_L _lf
            ;
_paren_r    : TK_PAREN_R
            ;
_bracket_l  : TK_BRACKET_L _lf
            ;
_bracket_r  : TK_BRACKET_R
            ;

start   : module { kModule = B2U($1); }
        ;
module  : module TK_LF create { $1->AddObj(B2U($3)); }
        | module TK_LF { $$ = $1; }
        | create { $$ = new ast::Module(kFilenames.front(), B2U($1)); }
        | %empty { $$ = new ast::Module(kFilenames.front()); }
        ;
create  : obj_create | function | assemble | struct | class | import
        ;
block   : TK_BRACE_L _stmts TK_BRACE_R { $$ = $2; }
        ;
_stmts  : _stmts TK_LF stmt { $1->AddStatement(B2U($3)); }
        | _stmts TK_LF { $$ = $1; }
        | stmt { $$ = new ast::Block(B2U($1));}
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
            | expr { $$ = new ast::ExprStatement(B2U($1)); }
            ;
obj_create  : TK_IDENTIFIER TK_CREATE expr { $$ = new ast::ObjCreate(B2U($1), B2U($3)); }
            ;
break       : TK_BREAK { $$ = new ast::Break(); }
            ;
continue    : TK_CONTINUE { $$ = new ast::Continue(); }
            ;
return      : TK_RETURN expr { $$ = new ast::Return(B2U($2)); }
            | TK_RETURN { $$ = new ast::Return(nullptr); }
            ;
raise       : TK_RAISE expr { $$ = new ast::Raise(B2U($2)); }
            ;
if          : TK_IF _paren_l logic_expr _paren_r block _end_if
                { $$ = new ast::If(B2U($3), B2U($5), B2U($6)); }
            ;
_end_if     : TK_ELSE block { $$ = $2; }
            | TK_ELSE TK_IF _paren_l logic_expr _paren_r block _end_if
                { $$ = new ast::If(B2U($4), B2U($6), B2U($7)); }
            | %empty { $$ = nullptr; }
            ;
while       : TK_WHILE _paren_l logic_expr _paren_r block TK_ELSE block
                { $$ = new ast::While(B2U($3), B2U($5), B2U($7)); }
            | TK_WHILE _paren_l logic_expr _paren_r block
                { $$ = new ast::While(B2U($3), B2U($5)); }
            ;
function    : TK_IDENTIFIER TK_CREATE TK_FUNC op_call block
                { $$ = new ast::Function(B2U($1), B2U($4), B2U($5)); }
            ;
assemble    : TK_IDENTIFIER TK_CREATE TK_ASM op_call block
                { $$ = new ast::Assemble(B2U($1), B2U($4), B2U($5)); }
            ;
struct      : TK_IDENTIFIER TK_CREATE TK_STRUCT _paren_l _paren_r block
                { $$ = new ast::Struct(B2U($1), B2U($6)); }
            ;
class       : TK_IDENTIFIER TK_CREATE TK_CLASS _paren_l _unamed_args _paren_r block
                { $$ = new ast::Class(B2U($1), B2U($5), B2U($7)); }
            ;
import      : TK_IDENTIFIER TK_CREATE TK_IMPORT _paren_l _unamed_args _paren_r block
                { $$ = new ast::Import(B2U($1), B2U($5), B2U($7)); }
            ;
try         : _beg_try TK_ELSE block { $$ = $1; $1->SetOrelse(B2U($3)); }
            | _beg_try { $$ = $1; }
            ;
_beg_try    : TK_TRY block TK_EXCEPT _paren_l TK_IDENTIFIER _comma name _paren_r block
                { $$ = new ast::Try(B2U($2)); $$->AddExcept({B2U($5), B2U($7), B2U($9)}); }
            | _beg_try TK_EXCEPT _paren_l TK_IDENTIFIER _comma name _paren_r block
                { $$->AddExcept({B2U($4), B2U($6), B2U($8)}); }
            ;

expr        : literal
            | name  { $$ = $1; }
            | uop_expr
            | bop_expr
            | aop_expr
            | logic_expr { $$ = $1; }
            | call { $$ = $1; }
            | subscript
            | if_else
            | _paren_l expr _paren_r { $$ = $2; }
            ;
name        : expr TK_MEMBER TK_IDENTIFIER
                { $$ = new ast::Name(B2U($3), false, B2U($1)); }
            | expr TK_DEREF_MEMBER TK_IDENTIFIER
                { $$ = new ast::Name(B2U($3), true, B2U($1)); }
            | TK_IDENTIFIER { $$ = new ast::Name(B2U($1)); }
            ;
literal     : TK_INTEGER { $$ = new ast::Literal(B2U($1), ast::Literal::Type::kInt); }
            | TK_FLOAT { $$ = new ast::Literal(B2U($1), ast::Literal::Type::kFloat); }
            | TK_STRING { $$ = new ast::Literal(B2U($1), ast::Literal::Type::kString); }
            ;
call        : expr op_call { $$ = new ast::CallExpr(B2U($1), B2U($2)); }
            ;
subscript   : expr op_subscript { $$ = new ast::SubscriptExpr(B2U($1), B2U($2)); }
            ;
if_else     : expr TK_IF expr TK_ELSE expr
                { $$ = new ast::IfElseExpr(B2U($1), B2U($3), B2U($5)); }
            ;

op_call     : _paren_l _unamed_args _paren_r { $$ = $2; }
            ;
_unamed_args: expr _comma _unamed_args { $$ = $3; $$->AddUnamed(B2U($1)); }
            | expr { $$ = new ast::CallOperator(); $$->AddUnamed(B2U($1)); }
            | _type_args { $$ = $1; }
            ;
_type_args  : TK_IDENTIFIER TK_COLON name _comma _type_args
                { $$ = $5; $$->AddTypeArg(B2U($1), B2U($3)); }
            | TK_IDENTIFIER TK_COLON name
                { $$ = new ast::CallOperator(); $$->AddTypeArg(B2U($1), B2U($3)); }
            | _named_args { $$ = $1; }
            ;
_named_args : TK_IDENTIFIER TK_CREATE expr _comma _named_args
                { $$ = $5; $$->AddKeyword(B2U($1), B2U($3)); }
            | TK_IDENTIFIER TK_CREATE expr
                { $$ = new ast::CallOperator(); $$->AddKeyword(B2U($1), B2U($3)); }
            | %empty { $$ = new ast::CallOperator(); }
            ;

op_subscript    : _bracket_l _subscript_list _bracket_r { $$ = $2; }
                ;
_subscript_list : _subscript_list _comma _subscript_arg { $1->AddDim(B2U($3)); }
                | _subscript_arg { $$ = new ast::SubscriptOperator(); $$->AddDim(B2U($1)); }
                ;
_subscript_arg  : expr TK_COLON expr TK_COLON expr
                    { $$ = new ast::SubscriptOperator::SubscriptArg({B2U($1), B2U($3), B2U($5)}); }
                | TK_COLON expr TK_COLON expr
                    { $$ = new ast::SubscriptOperator::SubscriptArg({nullptr, B2U($2), B2U($4)}); }
                | expr TK_COLON TK_COLON expr
                    { $$ = new ast::SubscriptOperator::SubscriptArg({B2U($1), nullptr, B2U($4)}); }
                | expr TK_COLON expr _colon
                    { $$ = new ast::SubscriptOperator::SubscriptArg({B2U($1), B2U($3), nullptr}); }
                | expr _colon_pair
                    { $$ = new ast::SubscriptOperator::SubscriptArg({B2U($1), nullptr, nullptr}); }
                | TK_COLON expr _colon
                    { $$ = new ast::SubscriptOperator::SubscriptArg({nullptr, B2U($2), nullptr}); }
                | TK_COLON TK_COLON expr
                    { $$ = new ast::SubscriptOperator::SubscriptArg({nullptr, nullptr, B2U($3)}); }
                ;
_colon_pair     : TK_COLON TK_COLON
                | TK_COLON
                | %empty
                ;
_colon          : TK_COLON
                | %empty
                ;

uop_expr    : TK_BNOT expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpBitNot()), B2U($2)); }
            | TK_NOT expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpNot()), B2U($2)); }
            | TK_PLUS expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpPositive()), B2U($2)); }
            | TK_MINUS expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpNegative()), B2U($2)); }
            | TK_MUL expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpDeref()), B2U($2)); }
            | TK_BAND expr %prec PR_UOP { $$ = new ast::UnaryOpExpr(UNEW(ast::OpRef()), B2U($2)); }
            ;

bop_expr    : expr TK_PLUS expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpPlus()), B2U($3)); }
            | expr TK_MINUS expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpMinus()), B2U($3)); }
            | expr TK_MUL expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpMul()), B2U($3)); }
            | expr TK_DIV expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpMod()), B2U($3)); }
            | expr TK_MOD expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpDiv()), B2U($3)); }
            | expr TK_BXOR expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpBitXor()), B2U($3)); }
            | expr TK_BOR expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpBitOr()), B2U($3)); }
            | expr TK_BAND expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpBitAnd()), B2U($3)); }
            | expr TK_SHIFT_L expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpShiftL()), B2U($3)); }
            | expr TK_SHIFT_R expr { $$ = new ast::BinaryOpExpr(B2U($1), UNEW(ast::OpShiftR()), B2U($3)); }

aop_expr    : name TK_ASSIGN expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpAssign()), B2U($3)); }
            | name TK_SELF_PLUS expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfPlus()), B2U($3)); }
            | name TK_SELF_MINUS expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfMinus()), B2U($3)); }
            | name TK_SELF_MUL expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfMul()), B2U($3)); }
            | name TK_SELF_DIV expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfMod()), B2U($3)); }
            | name TK_SELF_MOD expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfDiv()), B2U($3)); }
            | name TK_SELF_BXOR expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfBitXor()), B2U($3)); }
            | name TK_SELF_BOR expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfBitOr()), B2U($3)); }
            | name TK_SELF_BAND expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfBitAnd()), B2U($3)); }
            | name TK_SELF_SHIFT_L expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfShiftL()), B2U($3)); }
            | name TK_SELF_SHIFT_R expr { $$ = new ast::AssignOpExpr(B2U($1), UNEW(ast::OpSelfShiftR()), B2U($3)); }
            ;

logic_expr  : expr TK_OR expr { $$ = new ast::LogicExpr(B2U($1), UNEW(ast::OpOr()), B2U($3)); }
            | expr TK_AND expr { $$ = new ast::LogicExpr(B2U($1), UNEW(ast::OpAnd()), B2U($3)); }
            | expr TK_EQ expr { $$ = new ast::LogicExpr(B2U($1), UNEW(ast::OpEq()), B2U($3)); }
            | expr TK_NE expr { $$ = new ast::LogicExpr(B2U($1), UNEW(ast::OpNe()), B2U($3)); }
            | expr TK_LE expr { $$ = new ast::LogicExpr(B2U($1), UNEW(ast::OpLe()), B2U($3)); }
            | expr TK_GE expr { $$ = new ast::LogicExpr(B2U($1), UNEW(ast::OpGe()), B2U($3)); }
            | expr TK_LT expr { $$ = new ast::LogicExpr(B2U($1), UNEW(ast::OpLt()), B2U($3)); }
            | expr TK_GT expr { $$ = new ast::LogicExpr(B2U($1), UNEW(ast::OpGt()), B2U($3)); }
            ;
%%

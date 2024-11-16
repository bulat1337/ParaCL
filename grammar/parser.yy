%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.2"
%header

%define api.token.raw
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.trace
%define parse.error detailed
%define parse.lac full

%locations

%code requires
{
	class Driver;

	#include <string>
	#include "node.hh"
	#include "ast.hh"
}

%param { Driver& drv }

%code
{
	#include <memory>

	#include "log.h"
	#include "driver.hh"
}

%define api.token.prefix {TOK_}
%token
	ASSIGN		"="
	MINUS		"-"
	PLUS		"+"
	STAR		"*"
	SLASH		"/"
	LPAREN		"("
	RPAREN		")"
	READ		"?"
	PRINT		"print"
	LCPAREN		"{"
	RCPAREN		"}"
	SEMIC		";"
	IF			"if"
	WHILE		"while"
	GREATER		">"
	LESS		"<"
	GREATER_E	">="
	LESS_E		"<="
	EQUAL		"=="
	NOT_EQUAL	"!="
	NOT			"!"
;

%token <std::string>	ID		"identifier"
%token <int> 			NUMBER 	"number"

// ----- Statement derived -----
%nterm <std::unique_ptr<AST::ExpressionNode>> 	Expr
%nterm <std::unique_ptr<AST::UnaryOpNode>> 		UnaryOp
%nterm <std::unique_ptr<AST::BinaryOpNode>> 	BinaryOp
%nterm <std::unique_ptr<AST::AssignNode>> 		Assign
%nterm <std::unique_ptr<AST::ScopeNode>> 		Scope
%nterm <std::unique_ptr<AST::PrintNode>> 		Print
%nterm <std::unique_ptr<AST::IfNode>> 			If_Stm
%nterm <std::unique_ptr<AST::VariableNode>> 	Variable

%nterm <std::unique_ptr<AST::StatementNode>>	Statement

%printer { yyo << $$; } <*>;

%nonassoc "if"
%nonassoc "print"
%left "="
%left "+" "-"
%left "*" "/"
%nonassoc UMINUS NOT

%%

Program: /* nothing */
	   | 	Statements YYEOF
	   		{
				drv.ast.globalScope =
					std::make_unique<AST::ScopeNode>(std::move(drv.stm_table[drv.cur_scope_id]));
			}
	   ;

Statements: Statement				{ drv.stm_table[drv.cur_scope_id].push_back(std::move($1)); }
		  | Statements Statement	{ drv.stm_table[drv.cur_scope_id].push_back(std::move($2)); }
		  ;

Statement: Expr ";"
		 | Scope
		 | Assign ";"
		 | If_Stm
		 | Print ";"	{ $$ = std::move($1); }
		 ;

Scope: 	StartScope Statements EndScope
		{
			$$ = std::make_unique<AST::ScopeNode>(std::move(drv.stm_table[drv.cur_scope_id]));
		};

StartScope: "{"
			{
				++drv.cur_scope_id;
				drv.stm_table.resize(drv.stm_table.size() + 1);
			};

EndScope: 	"}"
			{
				--drv.cur_scope_id;
				drv.stm_table.pop_back();
			};

If_Stm: 	IF "(" Expr ")" Statement
			{
				$$ = std::make_unique<AST::IfNode>(std::move($3), std::move($5));
			};

Assign: Variable "=" Expr
		{
			$$ = std::make_unique<AST::AssignNode>(std::move($1), std::move($3));
		};

Print: "print" Expr { $$ = std::make_unique<AST::PrintNode>(std::move($2)); }


Expr:	BinaryOp		{ $$ = std::move($1); }
	|	UnaryOp			{ $$ = std::move($1); }
  	| 	"(" Expr ")"	{ $$ = std::move($2); }
  	| 	NUMBER			{ $$ = std::make_unique<AST::ConstantNode>($1); }
	| 	"?"				{ $$ = std::make_unique<AST::InNode>(); }
  	| 	Variable 		{ $$ = std::move($1); }
	;

BinaryOp: 	Expr "+" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::ADD,
															std::move($3));
			}
		| 	Expr "-" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::SUB,
															std::move($3));
			}
		| 	Expr "*" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::MUL,
															std::move($3));
			}
		| 	Expr "/" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::DIV,
															std::move($3));
			}
		|	Expr ">" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::GR,
															std::move($3));
			}
		|	Expr "<" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::LS,
															std::move($3));
			}
		|	Expr ">=" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::GR_EQ,
															std::move($3));
			}
		|	Expr "<=" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::LS_EQ,
															std::move($3));
			}
		|	Expr "==" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::EQ,
															std::move($3));
			}
		|	Expr "!=" Expr
			{
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::NOT_EQ,
															std::move($3));
			}
		;


UnaryOp	: 	"-" Expr %prec UMINUS
			{
				$$ = std::make_unique<AST::UnaryOpNode>(std::move($2), AST::UnaryOp::NEG);
			}
	 	| 	"!" Expr %prec NOT
			{
				$$ = std::make_unique<AST::UnaryOpNode>(std::move($2), AST::UnaryOp::NOT);
			}
	 	;

Variable: ID { $$ = std::make_unique<AST::VariableNode>(std::string($1)); };

%%

void yy::parser::error (const location_type& loc, const std::string& msg)
{
	std::cerr << loc << ": " << msg << '\n';
}

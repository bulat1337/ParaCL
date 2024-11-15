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
	#include <string>
	class Driver;
}

%param { Driver& drv }

%code
{
	#include "log.h"
	#include "driver.hh"
	#include "node.hh"
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

%token <std::unique_ptr<AST::VariableNode>> ID		"identifier"
%token <std::unique_ptr<AST::ConstantNode>> NUMBER 	"number"

// ----- Statement derived -----
// NOTE: No ExprNode in AST yet
%nterm <std::unique_ptr<AST::ExprNode> 			Expr
%nterm <std::unique_ptr<AST::UnaryOpNode> 		UnaryOp
%nterm <std::unique_ptr<AST::BinaryOpNode> 		BinaryOp
%nterm <std::unique_ptr<AST::AssignNode>> 		Assign
%nterm <std::unique_ptr<AST::ScopeNode>> 		Scope
%nterm <std::unique_ptr<AST::PrintNode>> 		Print
%nterm <std::unique_ptr<AST::IfNode>> 			If_Stm

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
	   | Statements YYEOF
	   ;

Statements: Statement
		  | Statements Statement
		  ;

Statement: /* nothing */
		 | Expr ";"
		 | Scope
		 | Assign ";"
		 | If_Stm
		 | Print ";"	{ drv.scopes[cur_scope_id].push_back($$); }
		 ;

Scope: 	StartScope Statements EndScope
		{
			$$ = std::make_unique<AST::ScopeNode>(drv.scopes[cur_scope_id]);
		};

StartScope: "{"
			{
				++drv.cur_scope_id;
				drv.scopes.push_back(AST::ScopeNode{});
			};

EndScope: 	"}"
			{
				--drv.cur_scope_id;
				drv.scopes.pop_back();
			};

If_Stm: 	IF "(" Expr ")" Statement
			{
				$$ = std::make_unique<AST::IfNode>(std::move($3), std::move($5));
			};

Assign: ID "=" Expr
		{
			$$ = std::make_unique<AST::AssignNode>(std::move($1), std::move($3));
		};

Print: "print" Expr { $$ = std::make_unique<AST::PrintNode>(std::move(Expr)); }


Expr:	BinaryOp		{ $$ = std::make_unique<AST::BinaryOp>($1); }
	|	UnaryOp			{ $$ = std::make_unique<AST::UnaryOp>($1); }
  	| 	"(" Expr ")"	{ $$ = std::move($2); }
  	| 	NUMBER			{ $$ = std::make_unique<AST::ConstantNode>($1); }
	| 	"?"				{ $$ = std::make_unique<AST::InNode>(); }
  	| 	ID 				{ $$ = std::make_unique<AST::VariableNode>($1); }
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

%%

void yy::parser::error (const location_type& loc, const std::string& msg)
{
	std::cerr << loc << ": " << msg << '\n';
}

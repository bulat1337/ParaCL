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
				MSG("Initialising global scope with vector of statements:\n");
				for (const auto& stm : drv.stm_table[drv.cur_scope_id])
				{
					LOG("{}\n", static_cast<const void*>(stm.get()));
				}

				drv.ast.globalScope =
					std::make_unique<AST::ScopeNode>(std::move(drv.stm_table[drv.cur_scope_id]));
			}
	   ;

Statements: Statement
			{
				LOG("Pushing statement : {}\n",
					static_cast<const void*>($1.get()));

				drv.stm_table[drv.cur_scope_id].push_back(std::move($1));
			}
		  | Statements Statement
		  	{
				LOG("Pushing statement : {}\n",
					static_cast<const void*>($2.get()));

				drv.stm_table[drv.cur_scope_id].push_back(std::move($2));
			}
		  ;

Statement: 	Expr ";"
			{
				LOG("It's Expr. Moving from concrete rule: {}\n",
					static_cast<const void*>($$.get()));

				$$ = std::move($1);
			}
		 | 	Scope
		 	{
				LOG("It's Scope. Moving from concrete rule: {}\n",
					static_cast<const void*>($$.get()));

				$$ = std::move($1);
			}
		 | 	Assign ";"
		 	{
				LOG("It's Assign. Moving from concrete rule: {}\n",
					static_cast<const void*>($$.get()));

				$$ = std::move($1);
			}
		 | 	If_Stm
		 	{
				LOG("It's If_Stm. Moving from concrete rule: {}\n",
					static_cast<const void*>($$.get()));

				$$ = std::move($1);
			}
		 | 	Print ";"
		 	{
				LOG("It's Print. Moving from concrete rule: {}\n",
					static_cast<const void*>($$.get()));

				$$ = std::move($1);
			}
		 ;

Scope: 	StartScope Statements EndScope
		{
			MSG("Initialising scope with vector of statements:\n");
			for (const auto& stm : drv.stm_table[drv.cur_scope_id])
			{
				LOG("{}\n", static_cast<const void*>(stm.get()));
			}

			$$ = std::make_unique<AST::ScopeNode>(std::move(drv.stm_table[drv.cur_scope_id]));

			MSG("Scope end.\n");

			--drv.cur_scope_id;

			LOG("drv.cur_scope_id is now {}\n", drv.cur_scope_id);

			drv.stm_table.pop_back();
		};

StartScope: "{"
			{
				MSG("Scope start.\n");

				++drv.cur_scope_id;

				LOG("drv.cur_scope_id is now {}\n", drv.cur_scope_id);

				drv.stm_table.resize(drv.stm_table.size() + 1);
			};

EndScope: 	"}"
			{

			};

If_Stm: 	IF "(" Expr ")" Statement
			{
				MSG("Initialising if statement\n");
				$$ = std::make_unique<AST::IfNode>(std::move($3), std::move($5));
			};

Assign: Variable "=" Expr
		{
			$$ = std::make_unique<AST::AssignNode>(std::move($1), std::move($3));
			LOG("Initialising assignment: {}\n", static_cast<const void*>($$.get()));
		};

Print: 	"print" Expr
		{
			MSG("Initialising print\n");
			$$ = std::make_unique<AST::PrintNode>(std::move($2));
		}


Expr:	BinaryOp
		{
			MSG("Moving BinaryOp\n");
			$$ = std::move($1);
		}
	|	UnaryOp
		{
			MSG("Moving UnaryOp\n");
			$$ = std::move($1);
		}
  	| 	"(" Expr ")"
		{
			MSG("Moving Expression in parenthesis\n");
			$$ = std::move($2);
		}
  	| 	NUMBER
		{
			MSG("Initialising ConstantNode\n");
			$$ = std::make_unique<AST::ConstantNode>($1);
		}
	| 	"?"
		{
			MSG("Initialising InNode\n");
			$$ = std::make_unique<AST::InNode>();
		}
  	| 	Variable
		{
			MSG("Moving VarialeNode\n");
			$$ = std::move($1);
		}
	;

BinaryOp: 	Expr "+" Expr
			{
				MSG("Initialising ADD operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::ADD,
															std::move($3));
			}
		| 	Expr "-" Expr
			{
				MSG("Initialising SUB operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::SUB,
															std::move($3));
			}
		| 	Expr "*" Expr
			{
				MSG("Initialising MUL operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::MUL,
															std::move($3));
			}
		| 	Expr "/" Expr
			{
				MSG("Initialising DIV operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::DIV,
															std::move($3));
			}
		|	Expr ">" Expr
			{
				MSG("Initialising GR operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::GR,
															std::move($3));
			}
		|	Expr "<" Expr
			{
				MSG("Initialising LS operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::LS,
															std::move($3));
			}
		|	Expr ">=" Expr
			{
				MSG("Initialising RG_EQ operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::GR_EQ,
															std::move($3));
			}
		|	Expr "<=" Expr
			{
				MSG("Initialising LS_EQ operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::LS_EQ,
															std::move($3));
			}
		|	Expr "==" Expr
			{
				MSG("Initialising EQ operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::EQ,
															std::move($3));
			}
		|	Expr "!=" Expr
			{
				MSG("Initialising NOT_EQ operation\n");
				$$ = std::make_unique<AST::BinaryOpNode>(	std::move($1),
															AST::BinaryOp::NOT_EQ,
															std::move($3));
			}
		;


UnaryOp	: 	"-" Expr %prec UMINUS
			{
				MSG("Initialising NEG operation\n");
				$$ = std::make_unique<AST::UnaryOpNode>(std::move($2), AST::UnaryOp::NEG);
			}
	 	| 	"!" Expr %prec NOT
			{
				MSG("Initialising NOT operation\n");
				$$ = std::make_unique<AST::UnaryOpNode>(std::move($2), AST::UnaryOp::NOT);
			}
	 	;

Variable: 	ID
			{
				MSG("Initialising VariableNode\n");
				$$ = std::make_unique<AST::VariableNode>(std::string($1));
			};

%%

void yy::parser::error (const location_type& loc, const std::string& msg)
{
	std::cerr << loc << ": " << msg << '\n';
}

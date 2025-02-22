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

	#include <exception>
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
	ELSE		"else"
	ELSEIF		"else if"
	WHILE		"while"
	GREATER		">"
	LESS		"<"
	GREATER_E	">="
	LESS_E		"<="
	EQUAL		"=="
	NOT_EQUAL	"!="
	NOT			"!"
	MOD			"%"
	AND			"&&"
	OR 			"||"
;

%token <std::string>	ID		"identifier"
%token <int> 			NUMBER 	"number"

// ----- Statement derived -----
%nterm <AST::ExpressionNode*> 	Expr
%nterm <AST::UnaryOpNode*> 		UnaryOp
%nterm <AST::BinaryOpNode*> 	BinaryOp
%nterm <AST::AssignNode*> 		Assign
%nterm <AST::ScopeNode*> 		Scope
%nterm <AST::PrintNode*> 		Print
%nterm <AST::IfNode*> 			If_Stm
%nterm <AST::ElseLikeNode*> 	Else_Like
%nterm <AST::ElseNode*> 		Else
%nterm <AST::ElseIfNode*> 		Else_If
%nterm <AST::WhileNode*> 		While_Stm
%nterm <AST::VariableNode*> 	Variable

%nterm <AST::StatementNode*>	Statement

%printer { yyo << $$; } <*>;

%nonassoc "if"
%nonassoc "print"
%left "="
%left "+" "-"
%left "*" "/"
%nonassoc UMINUS NOT

%%

Program: 	Statements YYEOF
	   		{
				MSG("Initialising global scope with vector of statements:\n");
				for (const auto& stm : drv.stm_table[drv.cur_scope_id])
				{
					LOG("{}\n", static_cast<const void*>(stm));
				}

				drv.ast.globalScope =
					drv.ast.construct<AST::ScopeNode>(std::move(drv.stm_table[drv.cur_scope_id]));
			}
	   ;

Statements: Statement
			{
				auto stm = $1;

				LOG("Pushing statement : {}\n",
					static_cast<const void*>(stm));

				if (stm)
					drv.stm_table[drv.cur_scope_id].push_back(stm);
			}
		  | Statements Statement
		  	{
				auto stm = $2;

				LOG("Pushing statement : {}\n",
					static_cast<const void*>(stm));

				if (stm)
					drv.stm_table[drv.cur_scope_id].push_back(stm);
			}
		  ;

Statement:	/* nothing */
			{
				MSG("Void statement\n");
				$$ = nullptr;
			}
		 |	";"
		 	{
				MSG("Lone semicolon\n");
				$$ = nullptr;
			}
		 |	Expr ";"
			{
				LOG("It's Expr. Moving from concrete rule: {}\n",
					static_cast<const void*>($$));

				$$ = $1;
			}
		 | 	Scope
		 	{
				LOG("It's Scope. Moving from concrete rule: {}\n",
					static_cast<const void*>($$));

				$$ = $1;
			}
		 | 	If_Stm
		 	{
				LOG("It's If_Stm. Moving from concrete rule: {}\n",
					static_cast<const void*>($$));

				$$ = $1;
			}
		|
			While_Stm
			{
				LOG("It's While_Stm. Moving from concrete rule: {}\n",
					static_cast<const void*>($$));

				$$ = $1;
			}
		 | 	Print ";"
		 	{
				LOG("It's Print. Moving from concrete rule: {}\n",
					static_cast<const void*>($$));

				$$ = $1;
			}
		 ;

Scope: 	StartScope Statements EndScope
		{
			MSG("Initialising scope with vector of statements:\n");
			for (const auto& stm : drv.stm_table[drv.cur_scope_id])
			{
				LOG("{}\n", static_cast<const void*>(stm));
			}

			$$ = drv.ast.construct<AST::ScopeNode>(std::move(drv.stm_table[drv.cur_scope_id]));

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
				$$ = drv.ast.construct<AST::IfNode>($3, $5);
			}
	    |	IF "(" Expr ")" Statement Else_Like
	  		{
				$$ = drv.ast.construct<AST::IfNode>($3, $5, $6);
			}

Else_Like:	Else
			{
				$$ = $1;
			}
		|	Else_If
			{
				$$ = $1;
			}
		;

Else:		ELSE Statement
			{
				$$ = drv.ast.construct<AST::ElseNode>($2);
			}

Else_If:	ELSEIF "(" Expr ")" Statement
			{
				$$ = drv.ast.construct<AST::ElseIfNode>($3, $5);
			}
	  |		ELSEIF "(" Expr ")" Statement Else_Like
	  		{
				$$ = drv.ast.construct<AST::ElseIfNode>($3, $5, $6);
			}
	  ;

While_Stm:	WHILE "(" Expr ")" Statement
			{
				MSG("Initialising while statement\n");
				$$ = drv.ast.construct<AST::WhileNode>($3, $5);
			};

Assign: Variable "=" Expr
		{
			$$ = drv.ast.construct<AST::AssignNode>($1, $3);
			LOG("Initialising assignment: {}\n", static_cast<const void*>($$));
		};

Print: 	"print" Expr
		{
			MSG("Initialising print\n");
			$$ = drv.ast.construct<AST::PrintNode>($2);
		}


Expr:	BinaryOp
		{
			MSG("Moving BinaryOp\n");
			$$ = $1;
		}
	|	UnaryOp
		{
			MSG("Moving UnaryOp\n");
			$$ = $1;
		}
  	| 	"(" Expr ")"
		{
			MSG("Moving Expression in parenthesis\n");
			$$ = $2;
		}
  	| 	NUMBER
		{
			MSG("Initialising AST::ConstantNode\n");
			$$ = drv.ast.construct<AST::ConstantNode>($1);
		}
	| 	"?"
		{
			MSG("Initialising AST::InNode\n");
			$$ = drv.ast.construct<AST::InNode>();
		}
  	| 	Variable
		{
			MSG("Moving AST::VarialeNode\n");
			$$ = $1;
		}
	| 	Assign
		{
			LOG("It's Assign. Moving from concrete rule: {}\n",
				static_cast<const void*>($$));

			$$ = $1;
		}
	;

BinaryOp: 	Expr "+" Expr
			{
				MSG("Initialising ADD operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::ADD, $3);
			}
		| 	Expr "-" Expr
			{
				MSG("Initialising SUB operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::SUB, $3);
			}
		| 	Expr "*" Expr
			{
				MSG("Initialising MUL operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::MUL, $3);
			}
		| 	Expr "/" Expr
			{
				MSG("Initialising DIV operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::DIV, $3);
			}
		|	Expr ">" Expr
			{
				MSG("Initialising GR operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::GR, $3);
			}
		|	Expr "<" Expr
			{
				MSG("Initialising LS operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::LS, $3);
			}
		|	Expr ">=" Expr
			{
				MSG("Initialising RG_EQ operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::GR_EQ, $3);
			}
		|	Expr "<=" Expr
			{
				MSG("Initialising LS_EQ operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::LS_EQ, $3);
			}
		|	Expr "==" Expr
			{
				MSG("Initialising EQ operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::EQ, $3);
			}
		|	Expr "!=" Expr
			{
				MSG("Initialising NOT_EQ operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::NOT_EQ, $3);
			}
		|	Expr "%" Expr
			{
				MSG("Initialising MOD operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::MOD, $3);
			}
		|	Expr "&&" Expr
			{
				MSG("Initialising AND operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::AND, $3);
			}
		|	Expr "||" Expr
			{
				MSG("Initialising OR operation\n");
				$$ = drv.ast.construct<AST::BinaryOpNode>($1, AST::BinaryOp::OR, $3);
			}
		;


UnaryOp	: 	"-" Expr %prec UMINUS
			{
				MSG("Initialising NEG operation\n");
				$$ = drv.ast.construct<AST::UnaryOpNode>($2, AST::UnaryOp::NEG);
			}
	 	| 	"!" Expr %prec NOT
			{
				MSG("Initialising NOT operation\n");
				$$ = drv.ast.construct<AST::UnaryOpNode>($2, AST::UnaryOp::NOT);
			}
	 	;

Variable: 	ID
			{
				MSG("Initialising AST::VariableNode\n");
				$$ = drv.ast.construct<AST::VariableNode>(std::string($1));
			};

%%

void yy::parser::error (const location_type& loc, const std::string& msg)
{
	std::cerr << loc << ": " << msg << '\n';
	throw std::runtime_error("Semantic error\n");
}

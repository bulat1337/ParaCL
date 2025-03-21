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

	#include "log.hh"
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
	REPEAT		"repeat"
	UNDEF		"undef"
	LSPAREN		"["
	RSPAREN		"]"
	COMMA		","
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
%nterm <AST::IfElseNode*> 		IfStm
%nterm <AST::IfElseNode*> 		ElseLike
%nterm <AST::IfElseNode*> 		Else
%nterm <AST::IfElseNode*> 		Else_If
%nterm <AST::WhileNode*> 		WhileStm
%nterm <AST::VariableNode*> 	Variable
%nterm <AST::ArrayElemNode*>	ArrayElem
%nterm <AST::RepeatNode*>		Repeat

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
				for ([[maybe_unused]]const auto& stm : drv.curScope())
				{
					LOG("{}\n", static_cast<const void*>(stm));
				}

				drv.formGlobalScope();
			}
	   ;

Statements: Statement
			{
				auto stm = $1;

				LOG("Pushing statement : {}\n",
					static_cast<const void*>(stm));


				drv.curScope().push_back(stm);
			}
		|	Statements Statement
		  	{
				auto stm = $2;

				LOG("Pushing statement : {}\n",
					static_cast<const void*>(stm));


				drv.curScope().push_back(stm);
			}
		| 	/* nothing */
		  	{
				MSG("Void statement\n");
			}
		|	";"
		 	{
				MSG("Lone semicolon\n");
			}
		;

Statement:	Expr ";"
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
		| 	IfStm
		 	{
				LOG("It's IfStm. Moving from concrete rule: {}\n",
					static_cast<const void*>($$));

				$$ = $1;
			}
		|
			WhileStm
			{
				LOG("It's WhileStm. Moving from concrete rule: {}\n",
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
			for ([[maybe_unused]]const auto& stm : drv.curScope())
			{
				LOG("{}\n", static_cast<const void*>(stm));
			}

			$$ = drv.formScope();

			MSG("Scope end.\n");

			drv.popScope();
		};

StartScope: "{"
			{
				MSG("Scope start.\n");

				drv.initScope();
			};

EndScope: 	"}"
			{

			};

IfStm: 	IF "(" Expr ")" Statement
			{
				MSG("Initialising if statement\n");
				$$ = drv.construct<AST::IfElseNode>($3, $5);
			}
	    |	IF "(" Expr ")" Statement ElseLike
	  		{
				$$ = drv.construct<AST::IfElseNode>($3, $5, $6);
			}

ElseLike:	ELSE Statement
			{
				$$ = drv.construct<AST::IfElseNode>($2);
			}
		|	ELSEIF "(" Expr ")" Statement
			{
				$$ = drv.construct<AST::IfElseNode>($3, $5);
			}
		|	ELSEIF "(" Expr ")" Statement ElseLike
			{
				$$ = drv.construct<AST::IfElseNode>($3, $5, $6);
			}
		;

WhileStm:	WHILE "(" Expr ")" Statement
			{
				MSG("Initialising while statement\n");
				$$ = drv.construct<AST::WhileNode>($3, $5);
			};

Assign: Variable "=" Expr
		{
			MSG("Constructing variable-expression Assign\n");

			$$ = drv.construct<AST::AssignNode>($1, $3);
		}
	|	Variable "=" Repeat
		{
			MSG("Constructing variable-repeat Assign\n");

			$$ = drv.construct<AST::AssignNode>($1, $3);
		}
	|	ArrayElem "=" Expr
		{
			MSG("Constructing arrayElem-expression Assign\n");

			$$ = drv.construct<AST::AssignNode>($1, $3);
		}
	|	ArrayElem "=" Repeat
		{
			MSG("Constructing arrayElem-repeat Assign\n");

			$$ = drv.construct<AST::AssignNode>($1, $3);
		}
	;

Repeat:	REPEAT LPAREN Expr COMMA Expr RPAREN
		{
			MSG("Constructing Repeat\n");

			$$ = drv.construct<AST::RepeatNode>($3, $5);
		}
	|
		REPEAT LPAREN UNDEF COMMA Expr RPAREN
		{
			MSG("Constructing Repeat with undefined element\n");

			$$ = drv.construct<AST::RepeatNode>($5);
		}
	;

ArrayElem: 	Variable LSPAREN Expr RSPAREN
			{
				MSG("Constructing ArrayElem from variable\n");

				$$ = drv.construct<AST::ArrayElemNode>($1, $3);
			}
		|	ArrayElem LSPAREN Expr RSPAREN
			{
				MSG("Constructing ArrayElem from another ArrayElem\n");

				$$ = drv.construct<AST::ArrayElemNode>($1, $3);
			}
		;

Print: 	"print" Expr
		{
			MSG("Initialising print\n");
			$$ = drv.construct<AST::PrintNode>($2);
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
			LOG("Initialising AST::ConstantNode: {}\n", $1);
			$$ = drv.construct<AST::ConstantNode>($1);
		}
	| 	"?"
		{
			MSG("Initialising AST::InNode\n");
			$$ = drv.construct<AST::InNode>();
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
	| 	ArrayElem
		{
			MSG("It's Array Elem\n");

			$$ = $1;
		}
	;

BinaryOp: 	Expr "+" Expr
			{
				MSG("Initialising ADD operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::ADD, $3);
			}
		| 	Expr "-" Expr
			{
				MSG("Initialising SUB operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::SUB, $3);
			}
		| 	Expr "*" Expr
			{
				MSG("Initialising MUL operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::MUL, $3);
			}
		| 	Expr "/" Expr
			{
				MSG("Initialising DIV operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::DIV, $3);
			}
		|	Expr ">" Expr
			{
				MSG("Initialising GR operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::GR, $3);
			}
		|	Expr "<" Expr
			{
				MSG("Initialising LS operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::LS, $3);
			}
		|	Expr ">=" Expr
			{
				MSG("Initialising RG_EQ operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::GR_EQ, $3);
			}
		|	Expr "<=" Expr
			{
				MSG("Initialising LS_EQ operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::LS_EQ, $3);
			}
		|	Expr "==" Expr
			{
				MSG("Initialising EQ operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::EQ, $3);
			}
		|	Expr "!=" Expr
			{
				MSG("Initialising NOT_EQ operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::NOT_EQ, $3);
			}
		|	Expr "%" Expr
			{
				MSG("Initialising MOD operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::MOD, $3);
			}
		|	Expr "&&" Expr
			{
				MSG("Initialising AND operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::AND, $3);
			}
		|	Expr "||" Expr
			{
				MSG("Initialising OR operation\n");
				$$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::OR, $3);
			}
		;


UnaryOp	: 	"-" Expr %prec UMINUS
			{
				MSG("Initialising NEG operation\n");
				$$ = drv.construct<AST::UnaryOpNode>($2, AST::UnaryOp::NEG);
			}
	 	| 	"!" Expr %prec NOT
			{
				MSG("Initialising NOT operation\n");
				$$ = drv.construct<AST::UnaryOpNode>($2, AST::UnaryOp::NOT);
			}
	 	;

Variable: 	ID
			{
				LOG("Initialising AST::VariableNode: {}\n", $1);
				$$ = drv.construct<AST::VariableNode>(drv.internName($1));
			};

%%

void yy::parser::error (const location_type& loc, const std::string& msg)
{
	std::cerr << loc << ": " << msg << '\n';
	throw std::runtime_error("Semantic error\n");
}

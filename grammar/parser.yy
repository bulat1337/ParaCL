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
;

%token <std::string> ID "identifier"
%token <int> NUMBER "number"

%nterm <int> Expr
%nterm <int> Assign

%printer { yyo << $$; } <*>;

%nonassoc "if"
%nonassoc "print"
%left "="
%left "+" "-";
%left "*" "/";
%nonassoc UMINUS

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
		 | Cond_Stm
		 | Std_Func ";"
		 ;

Scope: StartScope Statements EndScope
     | StartScope EndScope
     ;

StartScope: "{"
			{
				++drv.cur_scope_id;
				drv.var_table.push_back(Driver::Variables{});
			};

EndScope: 	"}"
			{
				--drv.cur_scope_id;
				drv.var_table.push_back(Driver::Variables{});
			};

Cond_Stm: 	IF "(" Expr ")" Statement { std::cout << "Ignoring Cond_Stm\n"; };

Assign: ID "=" Expr
		{
			MSG("Checking if assigned variable is already initialized\n");

			size_t scope_id = 0;

			while (scope_id != drv.cur_scope_id)
			{
				if (drv.var_table[scope_id].contains($1))
				{
					LOG("{} alredy initialized\n", $1);
					break;
				}
				++scope_id;
			}

			drv.var_table[scope_id][$1] = $3;

			$$ = $3;
		};

Std_Func: "print" Expr { drv.out << $2; }


Expr: 	Expr "+" Expr			{ $$ = $1 + $3; }
   	| 	Expr "-" Expr			{ $$ = $1 - $3; }
   	| 	Expr "*" Expr			{ $$ = $1 * $3; }
  	| 	Expr "/" Expr			{ $$ = $1 / $3; }
	|	Expr ">" Expr			{ $$ = $1 > $3; }
	|	Expr "<" Expr			{ $$ = $1 < $3; }
	|	Expr ">=" Expr			{ $$ = $1 >= $3; }
	|	Expr "<=" Expr			{ $$ = $1 <= $3; }
	|	Expr "==" Expr			{ $$ = $1 == $3; }
	|	Expr "!=" Expr			{ $$ = $1 != $3; }
  	| 	"(" Expr ")"			{ $$ = $2; }
  	| 	"-" Expr %prec UMINUS 	{ $$ = - $2; }
  	| 	NUMBER					{ $$ = $1; }
	| 	"?"						{ std::cin >> $$; }
  	| 	ID
		{
			for (int scope_id = static_cast<int>(drv.cur_scope_id); scope_id >= 0; --scope_id)
			{
				auto iter = drv.var_table[scope_id].find($1);

				if (iter == drv.var_table[scope_id].end())
					if (scope_id == 0)
						throw yy::parser::syntax_error(drv.location, "Unknown identifier\n");
					else continue;
				else
				{
					$$ = iter->second;
					break;
				}
			}
		};

%%

void yy::parser::error (const location_type& loc, const std::string& msg)
{
	std::cerr << loc << ": " << msg << '\n';
}

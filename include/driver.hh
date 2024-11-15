
#ifndef DRIVER_HH
#define DRIVER_HH

#include <string>
#include <unordered_map>
#include <ostream>

#include "parser.hh"
#include "ast.hh"

extern int yy_flex_debug;
extern int yydebug;
extern FILE* yyin;

#define YY_DECL \
	yy::parser::symbol_type yylex (Driver& drv)

YY_DECL;


class Driver final
{
  public: /* usings */
	using Variables = std::unordered_map<std::string, int>;

  public: /* members */
	std::string 	file;
	yy::location 	location;
	std::ostream& 	out;
	size_t 			cur_scope_id = 0;
	std::vector<AST::Scope> scopes;
	std::vector<Variables> var_table;



  public:

  	Driver(std::ostream& _out = std::cout):
		out(_out)
	{
		var_table.push_back(Variables{});
		scopes.push_back(AST::Scope{});
	}

	int parse(const std::string &f)
	{
		file = f;

		location.initialize(&file);

		scan_begin();

		yy::parser parse(*this);

		#if YYDEBUG
    	parse.set_debug_level(YYDEBUG);
		#endif

		int status = parse();

		scan_end();

		return status;
	}

	void scan_begin()
	{
		yy_flex_debug = YY_FLEX_DEBUG;

		if (file.empty () || file == "-") yyin = stdin;
		else if (!(yyin = fopen (file.c_str (), "r")))
		{
			std::cerr << "cannot open " << file << '\n';
			exit (EXIT_FAILURE);
		}
	}

	void scan_end ()
	{
		fclose (yyin);
	}
};
#endif // DRIVER_HH

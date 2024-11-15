
#ifndef DRIVER_HH
#define DRIVER_HH

#include <string>
#include <unordered_map>
#include <ostream>

#include "parser.hh"

extern int yy_flex_debug;
extern int yydebug;
extern FILE* yyin;

#define YY_DECL \
	yy::parser::symbol_type yylex (Driver& drv)

YY_DECL;


class Driver final
{
  public:
	std::string 	file;
	yy::location 	location;
	std::ostream& 	out;
	size_t 			cur_scope_id = 0;

	using Variables = std::unordered_map<std::string, int>;

	std::vector<Variables> var_table;

  public:

  	Driver(std::ostream& _out = std::cout):
		out(_out)
	{
		var_table.push_back(Variables{});
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

		int res = parse();

		scan_end();

		return res;
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

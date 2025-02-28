
#ifndef DRIVER_HH
#define DRIVER_HH

#include <ostream>
#include <string>
#include <unordered_map>

#include "ast.hh"
#include "parser.hh"

extern int yy_flex_debug;
extern int yydebug;
extern FILE *yyin;

#define YY_DECL yy::parser::symbol_type yylex(Driver &drv)

YY_DECL;

class Driver final
{
  public:
    yy::location location;

  private: /* members */
    std::string file_;
    AST::AST ast_;
    std::vector<std::vector<AST::StmtPtr>> stm_table_;

  public:
    Driver(std::ostream &out = std::cout)
        : ast_(out)
    {
        stm_table_.push_back(std::vector<AST::StmtPtr>());
    }

	const AST::ScopeNode* getGlobalScope() const
	{
		return ast_.globalScope;
	}

    void eval() { ast_.eval(); }

    template <typename NodeType, typename... Args>
    NodeType *construct(Args &&...args)
    {
        return ast_.construct<NodeType>(std::forward<Args>(args)...);
    }

    AST::ScopeNode *form_scope()
    {
        return construct<AST::ScopeNode>(std::move(stm_table_.back()));
    }

    auto &cur_scope() { return stm_table_.back(); }
    const auto &cur_scope() const { return stm_table_.back(); }

    void pop_scope() { stm_table_.pop_back(); }

    void init_scope() { stm_table_.emplace_back(); }

    void form_global_scope() { ast_.globalScope = form_scope(); }

    std::string_view intern_name(std::string_view name)
    {
        return ast_.intern_name(name);
    }

    int parse(const std::string &f)
    {
        file_ = f;

        location.initialize(&file_);

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

        if (file_.empty())
            yyin = stdin;
        else if (!(yyin = fopen(file_.c_str(), "r")))
            throw std::runtime_error("Can't open input file_\n");
    }

    void scan_end() { fclose(yyin); }
};
#endif // DRIVER_HH

#pragma once

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
    std::vector<std::vector<AST::StmtPtr>> stmTable_;

  public:
    Driver(std::ostream &out = std::cout)
        : ast_(out)
    {
        stmTable_.push_back(std::vector<AST::StmtPtr>());
    }

    const AST::ScopeNode *getGlobalScope() const { return ast_.globalScope; }

    void eval() { ast_.eval(); }

    template <typename NodeType, typename... Args>
    NodeType *construct(Args &&...args)
    {
        return ast_.construct<NodeType>(std::forward<Args>(args)...);
    }

    AST::ScopeNode *formScope()
    {
        return construct<AST::ScopeNode>(std::move(stmTable_.back()));
    }

    auto &curScope() { return stmTable_.back(); }
    const auto &curScope() const { return stmTable_.back(); }

    void popScope() { stmTable_.pop_back(); }

    void initScope() { stmTable_.emplace_back(); }

    void formGlobalScope() { ast_.globalScope = formScope(); }

    int getInterpreterBuf() const { return ast_.getInterpreterBuf(); }

    bool varInitialized(std::string_view varName) const
    {
        return ast_.varInitialized(varName);
    }

    std::string_view internName(std::string_view name)
    {
        return ast_.internName(name);
    }

    int parse(const std::string &f)
    {
        file_ = f;

        location.initialize(&file_);

        scanBegin();

        yy::parser parse(*this);

#if YYDEBUG
        parse.set_debug_level(YYDEBUG);
#endif

        int status = parse();

        scanEnd();

        return status;
    }

    void scanBegin()
    {
        yy_flex_debug = YY_FLEX_DEBUG;

        if (file_.empty())
            yyin = stdin;
        else if (!(yyin = fopen(file_.c_str(), "r")))
            throw std::runtime_error("Can't open input file_\n");
    }

    void scanEnd() { fclose(yyin); }
};

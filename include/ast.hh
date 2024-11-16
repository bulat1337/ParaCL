#ifndef AST_HH
#define AST_HH

#include "node.hh"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace AST 
{

class AST final
{
private:
    using VarTable = std::unordered_map<std::string, int>;

private:
    std::unique_ptr<ScopeNode> globalScope_;

    std::vector<VarTable> VarTables_;

    detail::Context ctx;

public:
    int eval()
    {
        return globalScope_->eval(ctx);
    }
};

} // namespace AST

#endif // !AST_HH

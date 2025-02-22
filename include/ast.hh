#ifndef AST_HH
#define AST_HH

#include "node.hh"
#include "log.h"

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

public:
    ScopeNode* globalScope;

private:

    std::vector<std::unique_ptr<INode>> data_;

    std::vector<VarTable> VarTables_;

    detail::Context ctx;

public:
	AST(std::ostream& out = std::cout):
		ctx(out) {}

    void eval()
    {
		MSG("Evaluating global scope\n");
        globalScope->eval(ctx);
    }

	template <typename NodeType, typename... Args>
	NodeType* construct(Args&&... args)
	{
		auto node_ptr = std::make_unique<NodeType>(std::forward<Args>(args)...);

		auto raw_data = node_ptr.get();

		data_.push_back(std::move(node_ptr));

		return raw_data;
	}
};

} // namespace AST

#endif // !AST_HH

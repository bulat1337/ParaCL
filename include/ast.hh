#pragma once

#include "interpreter.hh"
#include "log.hh"
#include "node.hh"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace AST
{

class AST final
{
  public:
    ScopeNode *globalScope;

  private:
    std::vector<std::unique_ptr<INode>> data_;

    std::unordered_set<std::string> namePool_;

    detail::Interpreter interpreter_;

  public:
    AST(std::ostream &out = std::cout)
        : interpreter_(out)
    {}

    void eval()
    {
        MSG("Evaluating global scope\n");
        interpreter_.visit(*globalScope);
    }

    template <typename NodeType, typename... Args>
    NodeType *construct(Args &&...args)
    {
        auto node_ptr = std::make_unique<NodeType>(std::forward<Args>(args)...);

        auto raw_data = node_ptr.get();

        data_.push_back(std::move(node_ptr));

        return raw_data;
    }

    std::string_view internName(std::string_view name)
    {
        const auto it = namePool_.insert(std::string(name)).first;

        return *it;
    }

    int getInterpreterBuf() const { return interpreter_.getBuf(); }

    bool varInitialized(std::string_view varName) const
    {
        return interpreter_.varInitialized(varName);
    }
};

} // namespace AST

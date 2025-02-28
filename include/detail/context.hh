#ifndef CONTEXT_HH
#define CONTEXT_HH

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "log.h"

namespace AST
{

namespace detail
{

class Context final
{
  public:
    using VarTable = std::unordered_map<std::string_view, int>;

  public:
    std::vector<VarTable> varTables_;
    int32_t curScope_ = -1;
    std::ostream &out;

  public:
    Context(std::ostream &_out = std::cout)
        : out(_out)
    {}

    int getVarValue(std::string_view name) const
    {
        auto iter = varTables_.rbegin() + (varTables_.size() - curScope_ - 1);
        auto rend = varTables_.rend();

        for (; iter != rend; ++iter)
        {
            auto it = iter->find(name);

            if (it != iter->end())
            {
                LOG("It's {}\n", it->second);
                return it->second;
            }
        }

        throw std::runtime_error("Undeclared variable: " + std::string(name) +
                                 "\n");
    }

    auto& get_variable(std::string_view destName)
    {
        int32_t scopeId = 0;

        while (scopeId < curScope_)
        {
            if (varTables_[scopeId].contains(destName))
                break;
            scopeId++;
        }

        return varTables_[scopeId][destName];
    }
};

} // namespace detail
} // namespace AST

#endif // !CONTEXT_HH

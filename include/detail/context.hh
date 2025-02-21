#ifndef CONTEXT_HH
#define CONTEXT_HH

#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <optional>

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
	std::ostream& out;

  public:
	Context(std::ostream& _out = std::cout):
		out(_out) {}

	int getVarValue(std::string_view name) const
	{
		for (int32_t scopeId = curScope_; scopeId >= 0; --scopeId)
        {
            auto it = varTables_[scopeId].find(name);

            if (it != varTables_[scopeId].end())
            {
				LOG("It's {}\n", it->second);
                return it->second;
            }
        }

		throw std::runtime_error("Undeclared variable: " + std::string(name) + "\n");
	}
};

} // namespace detail
} // namespace AST

#endif // !CONTEXT_HH

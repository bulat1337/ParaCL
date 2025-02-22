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
		auto iter = varTables_.rbegin() + (varTables_.size() - curScope_ - 1);
		auto rend = varTables_.rend();

		for (;iter != rend; ++iter)
		{
            auto it = iter->find(name);

            if (it != iter->end())
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

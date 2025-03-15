#pragma once

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "log.hh"
#include "types.hh"

namespace AST
{

namespace detail
{

class Context final
{
  public:
    using VarTable = std::unordered_map<std::string_view, std::unique_ptr<IType>>;

  public:
    std::vector<VarTable> varTables_;
    std::ostream &out;

  public:
    Context(std::ostream &_out = std::cout)
        : out(_out)
    {}

    IType* getVarValue(std::string_view name) const
    {
        auto iter = varTables_.rbegin();
        auto rend = varTables_.rend();

        for (; iter != rend; ++iter)
        {
            auto it = iter->find(name);

            if (it != iter->end())
            	return it->second.get();
        }

        throw std::runtime_error("Undeclared variable: " + std::string(name) +
                                 "\n");
    }

	template <typename T>
    T* getVar(std::string_view destName)
    {
		for (auto iter = varTables_.rbegin() ; iter != varTables_.rend() ; ++iter)
		{
			auto var_iter = iter->find(destName);
			if (var_iter != iter->end())
			{
				auto ptr = dynamic_cast<T*>(var_iter->second.get());

				if (!ptr) throw std::runtime_error("Invalid type\n");

				return ptr;
			}
		}

		if (varTables_.empty())
			throw std::runtime_error("No active scope\n");

		auto& curScope = varTables_.back();
		auto [iter, inserted] = curScope.emplace(destName, std::make_unique<T>());

		if (!inserted)
			throw std::logic_error(	"Undeclared variable was supposed to be inserted:"
									"Check code logic\n");

		return static_cast<T*>(iter->second.get());
    }
};

} // namespace detail
} // namespace AST

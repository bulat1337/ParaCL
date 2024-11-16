#ifndef CONTEXT_HH
#define CONTEXT_HH

#include <cstdint>
#include <sys/types.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace AST
{

namespace detail
{

class Context final
{
public:
    using VarTable = std::unordered_map<std::string, int>;

public:
    std::vector<VarTable> varTables_;
    int32_t curScope_ = -1;

public:

};

} // namespace detail
} // namespace AST

#endif // !CONTEXT_HH

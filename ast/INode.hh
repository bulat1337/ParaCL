#ifndef INODE_HH
#define INODE_HH

#include <algorithm>
#include <memory>
#include <cstddef>
#include <string>

namespace AST 
{
 
class INode;

using INodePtr = std::unique_ptr<INode>;

enum class BinaryOp
{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    GR,
    LS,
    EQ,
    GR_EQ,
    LS_EQ,
    NOT_EQ,
    AND,
    OR,
};

enum class UnaryOp
{
    NEG,
    NOT,
};

class INode
{
protected:
    size_t childCount_;

    INode(size_t childCount = 0) 
    :   childCount_(childCount) {} 

public:
    virtual INode* getChild(size_t i) const
    {
        return nullptr;
    }

    virtual int eval() const = 0;

    size_t getChildCount() const
    {
        return childCount_;
    }

    virtual ~INode() = default;
};

} // namespace AST


#endif // ! INODE_HH


#ifndef INODE_HH
#define INODE_HH

#include <memory>
#include <cstddef>
#include <string>
#include <unordered_map>

namespace AST 
{
 
class INode;

using INodePtr = std::unique_ptr<INode>;
using VarIterator = std::unordered_map<std::string, int>::iterator;
template<class T>
using ObserverPtr = T*;


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
    virtual const INode& getChild(size_t i) const = 0;

    virtual int eval() const = 0;

    size_t getChildCount() const
    {
        return childCount_;
    }

    virtual ~INode() = default;
};

class IScope : public INode
{
protected:
    IScope(size_t childCount)
    :   INode(childCount) {}

public:
    virtual const ObserverPtr<IScope>& resetScope()               = 0;
    virtual VarIterator getVariableIterator (const std::string& name) = 0;
    virtual void insertVariable (std::string name, int initialValue)  = 0;
    virtual void insertNode (INodePtr&& node)                    = 0;

    virtual ~IScope() override = default;
};

} // namespace AST

#endif // ! INODE_HH


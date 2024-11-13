#ifndef NODE_HH
#define NODE_HH

#include "INode.hh"
#include <cstddef>
#include <exception>

namespace AST
{

class ConstantNode final : public INode
{
private:
    const int val_;

public:
    ConstantNode(int val)
    :   val_(val) {}

    int eval() const override;
};

class VariableNode final : public INode
{
private:
    // TODO: some sort of iterator to VariableTable

public:

    int eval() const override;

    // TODO: some method that changes the value of the variable
};

class BinaryOpNode final : public INode
{
private:
    INodePtr left_;
    INodePtr right_;
    BinaryOp op_;

public:
    BinaryOpNode(const INodePtr& left, BinaryOp op, const INodePtr& right)
    :   INode(2),
        left_(std::move(left)),
        op_(op),
        right_(std::move(right)) {}

    INode* getChild(size_t i) const override
    {
        if (i > childCount_)
        {
            // TODO: error handling
        }

        return i == 0 ? left_.get() : right_.get();
    }

    int eval() const override;
};

class UnaryOpNode final : public INode
{
private:
    INodePtr operand_;
    UnaryOp op_;

public:
    UnaryOpNode(const INodePtr& operand, UnaryOp op)
    :   INode(1),
        operand_(std::move(operand)),
        op_(op) {}

    INode* getChild(size_t i) const override
    {
        // TODO: do we need to handle the input?
        return operand_.get();
    }

    int eval() const override;
};

class AssignNode final : public INode
{
private:
    std::unique_ptr<VariableNode> dest_;
    INodePtr expr_;

public:
    AssignNode(const std::unique_ptr<VariableNode>& dest_, const INodePtr& expr_)
    :   dest_(std::move(dest)),
        expr_(std::move(expr)) {}
    
    INode* getChild(size_t i) const override
    {
        if (i > childCount_)
        {
            // TODO: error handling
        }

        return i == 0 ? dest_.get() : expr_.get();
    }

    int eval() const override;
};

class WhileNode final : public INode
{
private:
    INodePtr cond_;
    INodePtr scope_;

public:
    WhileNode(const INodePtr& cond, const INodePtr& scope) 
    :   cond_(std::move(cond)),
        scope_(std::move(scope)) {}

    INode* getChild(size_t i) const override
    {
        if (i > childCount_)
        {
        }
    }

    int eval() const override;
};

class IfNode final : public INode
{
private:
    INodePtr cond_;
    INodePtr ifScope_;
    INodePtr elseScope_;

public:
    IfNode(const INodePtr& cond, const INodePtr& ifScope, const INodePtr& elseScope = nullptr)
    :   cond_(std::move(cond)),
        ifScope_(std::move(ifScope)),
        elseScope_(std::move(elseScope)) {}

    INode* getChild(size_t i) const override;

    int eval() const override;
};

class PrintNode final : public INode
{
private:
    INodePtr expr_;

public:
    PrintNode(const INodePtr& expr)
    :   expr_(std::move(expr)) {}

    INode* getChild(size_t i) const override;

    int eval() const override;
};


class InNode final : public INode
{
public:
    InNode() = default;

    INode* getChild(size_t i) const override;

    int eval() const override;

    ~InNode() = default;
};

} // namespace AST

#endif // ! NODE_HH

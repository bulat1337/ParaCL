#ifndef NODE_HH
#define NODE_HH

#include "INode.hh"
#include <cmath>
#include <cstddef>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace AST
{

class Scope final : public IScope
{
private:
    std::vector<INodePtr> nodes_;

    ObserverPtr<IScope> parent_;

    std::unordered_map<std::string, int> variableTable_;

public:
    Scope(IScope* parent) 
    :   IScope(0), parent_(parent) {}

    const ObserverPtr<IScope>& resetScope() override
    {
        return parent_;
    }

    void insertNode(INodePtr&& node) override
    {
        nodes_.push_back(std::move(node));

        childCount_ = nodes_.size();
    }

    const INode& getChild(size_t i) const override
    {
        return *nodes_.at(i).get();
    }

    void insertVariable(std::string name, int initialValue = 0) override 
    {
        variableTable_.emplace(name, initialValue);
    }

    VarIterator getVariableIterator(const std::string& name) override
    {
        auto it = variableTable_.find(name);

        if (it != variableTable_.end()) 
        {
            return it;
        }

        return variableTable_.end(); // TODO: ???
        // throw std::runtime_error("Variable not found: " + name);
    }

    int eval() const override {}

   ~Scope() override = default;
};

class ConstantNode final : public INode
{
private:
    const int val_;

public:
    ConstantNode(int val)
    :   INode(0), val_(val) {}

    int eval() const override
    {
        return val_;
    }

    const INode& getChild(size_t i) const override {}

    ~ConstantNode() override = default;
};

class VariableNode final : public INode
{
private:
    VarIterator varIt_;

public:
    VariableNode(VarIterator varIt)
    :   INode(0), varIt_(varIt) {}

    void setValue(int value)
    {
        varIt_->second = value;
    }

    int eval() const override
    {
        return varIt_->second;
    }

    const INode& getChild(size_t i) const override {}

    ~VariableNode() override = default;
};

class BinaryOpNode final : public INode
{
private:
    INodePtr left_;
    INodePtr right_;
    BinaryOp op_;

public:
    BinaryOpNode(INodePtr&& left, BinaryOp op, INodePtr&& right)
    :   INode(2),
        left_(std::move(left)),
        right_(std::move(right)),
        op_(op) {}

    const INode& getChild(size_t i) const override
    {
        if (i > childCount_)
        {
            // TODO: error handling
        }

        return i == 0 ? *left_.get() : *right_.get();
    }

    int eval() const override
    {
        int leftVal = left_->eval();
        int rightVal = right_->eval();

        switch (op_) 
        {
            case BinaryOp::ADD:
                return leftVal + rightVal;

            case BinaryOp::SUB:
                return leftVal - rightVal;

            case BinaryOp::MUL:
                return leftVal * rightVal;

            case BinaryOp::DIV:
                if (rightVal == 0) // TODO: error handle
                return leftVal / rightVal;

            case BinaryOp::MOD:
                return leftVal % rightVal;

            case BinaryOp::LS:
                return leftVal < rightVal;

            case BinaryOp::GR:
                return leftVal > rightVal;

            case BinaryOp::LS_EQ:
                return leftVal <= rightVal;

            case BinaryOp::GR_EQ:
                return leftVal >= rightVal;

            case BinaryOp::EQ:
                return leftVal == rightVal;

            case BinaryOp::NOT_EQ:
                return leftVal != rightVal;

            case BinaryOp::AND:
                return leftVal && rightVal;

            case BinaryOp::OR:
                return leftVal || rightVal;
                
            default:
                return -1; // FIXME: remove this
                // TODO: handle error
        }
    }
};

class UnaryOpNode final : public INode
{
private:
    INodePtr operand_;
    UnaryOp op_;

public:
    UnaryOpNode(INodePtr&& operand, UnaryOp op)
    :   INode(1),
        operand_(std::move(operand)),
        op_(op) {}

    const INode& getChild(size_t i) const override
    {
        // TODO: do we need to handle the input?
        return *operand_.get();
    }

    int eval() const override
    {
        int operandVal = operand_->eval();

        switch (op_)
        {
            case UnaryOp::NEG:
                return - operandVal;
            
            case UnaryOp::NOT:
                return ! operandVal;

            default:
                return -1; // FIXME: fixme
                // TODO: error handle
        }
    }
};

class AssignNode final : public INode
{
private:
    std::unique_ptr<VariableNode> dest_;
    INodePtr expr_;

public:
    AssignNode(std::unique_ptr<VariableNode>&& dest, INodePtr&& expr)
    :   dest_(std::move(dest)),
        expr_(std::move(expr)) {}
    
    const INode& getChild(size_t i) const override
    {
        if (i > childCount_)
        {
            // TODO: error handling
        }

        return i == 0 ? *dest_.get() : *expr_.get();
    }

    int eval() const override
    {
        int value = expr_->eval();

        dest_->setValue(value);

        return value;
    }
};

class WhileNode final : public INode
{
private:
    INodePtr cond_;
    INodePtr scope_;

public:
    WhileNode(INodePtr&& cond, INodePtr&& scope) 
    :   cond_(std::move(cond)),
        scope_(std::move(scope)) {}

    const INode& getChild(size_t i) const override
    {
        if (i > childCount_)
        {
        }

        return i == 0 ? *cond_.get() : *scope_.get();
    }

    int eval() const override
    {
        int result = 0;
        
        while (cond_->eval())
        {
            result = scope_->eval();
        }
        
        return result;
    }
};

class IfNode final : public INode
{
private:
    INodePtr cond_;
    INodePtr ifScope_;
    INodePtr elseScope_;

public:
    IfNode(INodePtr&& cond, INodePtr&& ifScope, INodePtr&& elseScope = nullptr)
    :   cond_(std::move(cond)),
        ifScope_(std::move(ifScope)),
        elseScope_(std::move(elseScope)) {}

    const INode& getChild(size_t i) const override
    {
        switch (i)
        {
            case 0:
                return *cond_.get();

            case 1:
                return *ifScope_.get();

            case 2:
                if (elseScope_)
                    return *elseScope_.get();

            default:
                return *cond_.get();


            
                // TODO: error handle
        }
    }

    int eval() const override
    {
        if (cond_->eval())
        {
            return ifScope_->eval();
        }
        else if (elseScope_)
        {
            return elseScope_->eval();
        }

        return 0;
    }
};

class PrintNode final : public INode
{
private:
    INodePtr expr_;

public:
    PrintNode(INodePtr&& expr)
    :   expr_(std::move(expr)) {}

    const INode& getChild(size_t i) const override;

    int eval() const override
    {
        int value = expr_->eval();

        std::cout << value << std::endl;

        return value;
    }
};


class InNode final : public INode
{
public:
    InNode() = default;

    const INode& getChild(size_t i) const override;

    int eval() const override
    {
        int value = 0;

        std::cin >> value;

        if (! std::cin.good())
        {
            // TODO: error handle
        }

        return value;
    }

    ~InNode() = default;
};

} // namespace AST

#endif // ! NODE_HH

#ifndef NODE_HH
#define NODE_HH

#include "detail/context.hh"
#include "detail/inode.hh"
#include "inode.hh"
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace AST
{

class StatementNode : public INode {};

class ExpressionNode : public StatementNode {};

class ConditionalStatementNode : public StatementNode {};

using ExprPtr = std::unique_ptr<ExpressionNode>;

using StmtPtr = std::unique_ptr<StatementNode>;

using CondStmtPtr = std::unique_ptr<ConditionalStatementNode>;

class ScopeNode final : public StatementNode
{
private:
    std::vector<StatementNode> children_;

public:
    int eval(detail::Context& ctx) const override 
    {
        ctx.curScope_++;

        ctx.varTables_.push_back(detail::Context::VarTable());

        for (auto child : children_)
        {
            child.eval(ctx);
        }

        ctx.varTables_.pop_back();

        ctx.curScope_--;
    }
};

class ConstantNode final : public ExpressionNode
{
private:
    const int val_;

public:
    ConstantNode(int val)
    :   val_(val) {}

    int eval(detail::Context& ctx) const override
    {
        return val_;
    }
};

class VariableNode final : public INode
{
private:
    std::string name_;

public:
    const std::string& getName() const
    {
        return name_;
    }

    int eval(detail::Context& ctx) const override
    {
        for (int32_t scopeId = ctx.curScope_; scopeId >= 0; --scopeId)
        {
            auto it = ctx.varTables_[scopeId].find(name_);

            if (it != ctx.varTables_[scopeId].end())
            {
                return it->second;
            }
            
            if (scopeId == 0)
            {
                throw std::logic_error("Undeclared variable");
            }
        }
    }
};

class BinaryOpNode final : public ExpressionNode
{
private:
    ExprPtr left_;
    ExprPtr right_;
    BinaryOp op_;

public:
    BinaryOpNode(ExprPtr&& left, BinaryOp op, ExprPtr&& right)
    :   left_(std::move(left)),
        right_(std::move(right)),
        op_(op) {}

    int eval(detail::Context& ctx) const override
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

class UnaryOpNode final : public ExpressionNode
{
private:
    ExprPtr operand_;
    UnaryOp op_;

public:
    UnaryOpNode(ExprPtr&& operand, UnaryOp op)
    :   operand_(std::move(operand)),
        op_(op) {}

    int eval(detail::Context& ctx) const override
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

class AssignNode final : public StatementNode
{
private:
    std::unique_ptr<VariableNode> dest_;
    ExprPtr expr_;

public:
    AssignNode(std::unique_ptr<VariableNode>&& dest, ExprPtr&& expr)
    :   dest_(std::move(dest)),
        expr_(std::move(expr)) {}

    int eval(detail::Context& ctx) const override
    {
        std::string destName = dest_->getName();

        int value = expr_->eval(ctx);

        int32_t scopeId = 0;

        while (scopeId <= ctx.curScope_)
            if (ctx.varTables_[scopeId].contains(destName)) break;

        ctx.varTables_[scopeId][destName] = value;

        return value;
    }
};

class WhileNode final : public ConditionalStatementNode
{
private:
    ExprPtr cond_;
    StmtPtr scope_;

public:
    WhileNode(ExprPtr&& cond, StmtPtr&& scope)
    :   cond_(std::move(cond)),
        scope_(std::move(scope)) {}

    int eval(detail::Context& ctx) const override
    {
        int result = 0;

        while (cond_->eval())
        {
            result = scope_->eval();
        }

        return result;
    }
};

class IfNode final : public ConditionalStatementNode
{
private:
    ExprPtr cond_;
    StmtPtr action_;
    StmtPtr altAction_;

public:
    IfNode(ExprPtr&& cond, StmtPtr&& action, StmtPtr&& altAction)
    :   cond_(std::move(cond)),
        action_(std::move(action)),
        altAction_(std::move(altAction)) {}

    int eval(detail::Context& ctx) const override
    {
        if (cond_->eval(ctx))
        {
            return action_->eval(ctx);
        }
        else
        {
            return altAction_->eval(ctx);
        }

        return 0;
    }
};

class PrintNode final : public StatementNode
{
private:
    ExprPtr expr_;

public:
    PrintNode(ExprPtr&& expr)
    :   expr_(std::move(expr)) {}

    int eval(detail::Context& ctx) const override
    {
        int value = expr_->eval(ctx);

        std::cout << value << std::endl;

        return value;
    }
};


class InNode final : public StatementNode
{
public:
    InNode() = default;

    int eval(detail::Context& ctx) const override
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

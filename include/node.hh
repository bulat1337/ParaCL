#ifndef NODE_HH
#define NODE_HH

#include "detail/context.hh"
#include "detail/inode.hh"
#include "inode.hh"
#include "log.h"

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
    std::vector<StmtPtr> children_;

public:
	ScopeNode(std::vector<StmtPtr>&& stms)
	{
		for (auto& stm : stms)
		{
			children_.push_back(std::move(stm));
		}
	}

    int eval(detail::Context& ctx) const override
    {
		MSG("Evaluating scope\n");

        ++ctx.curScope_;

        ctx.varTables_.push_back(detail::Context::VarTable());

		LOG("ctx.curScope_ = {}\n", ctx.curScope_);
		LOG("ctx.varTables_ size = {}\n", ctx.varTables_.size());

		MSG("Scopes children:\n");
		for ([[maybe_unused]]const auto& child : children_)
        {
			LOG("{}\n", static_cast<const void*>(child.get()));
        }

        for (const auto& child : children_)
        {
			LOG("Evaluating {}\n", static_cast<const void*>(child.get()));
            child->eval(ctx);
        }

        ctx.varTables_.pop_back();

        --ctx.curScope_;

		LOG("ctx.curScope_ = {}\n", ctx.curScope_);
		LOG("ctx.varTables_ size = {}\n", ctx.varTables_.size());

		// TODO: usless return value. Rework later.
		return 0;
    }

    void pushChild(StmtPtr&& stmt)
    {
        children_.push_back(std::move(stmt));
    }

	size_t nstms() const { return children_.size(); }
};

using ScopePtr = std::unique_ptr<ScopeNode>;

class ConstantNode final : public ExpressionNode
{
private:
    const int val_;

public:
    ConstantNode(int val)
    :   val_(val) {}

    int eval([[maybe_unused]]detail::Context& ctx) const override
    {
		LOG("Evaluating constant: {}\n", val_);
        return val_;
    }
};

class VariableNode final : public ExpressionNode
{
private:
    std::string name_;

public:
	VariableNode(const std::string& name): name_(name) {}

    const std::string& getName() const
    {
        return name_;
    }

    int eval(detail::Context& ctx) const override
    {
		LOG("Evaluating variable: {}\n", name_);

        for (int32_t scopeId = ctx.curScope_; scopeId >= 0; --scopeId)
        {
            auto it = ctx.varTables_[scopeId].find(name_);

            if (it != ctx.varTables_[scopeId].end())
            {
				LOG("It's {}\n", it->second);
                return it->second;
            }
        }

		throw std::logic_error("Undeclared variable");
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
		MSG("Evaluating Binary Operation\n");

        int leftVal = left_->eval(ctx);
        int rightVal = right_->eval(ctx);

		int result = 0;

        switch (op_)
        {
            case BinaryOp::ADD:
                result = leftVal + rightVal;
				break;

            case BinaryOp::SUB:
                result = leftVal - rightVal;
				break;

            case BinaryOp::MUL:
                result = leftVal * rightVal;
				break;

            case BinaryOp::DIV:
                if (rightVal == 0) { /* TODO: error handle */ }
                result = leftVal / rightVal;
				break;

            case BinaryOp::MOD:
                result = leftVal % rightVal;
				break;

            case BinaryOp::LS:
                result = leftVal < rightVal;
				break;

            case BinaryOp::GR:
                result = leftVal > rightVal;
				break;

            case BinaryOp::LS_EQ:
                result = leftVal <= rightVal;
				break;

            case BinaryOp::GR_EQ:
                result = leftVal >= rightVal;
				break;

            case BinaryOp::EQ:
                result = leftVal == rightVal;
				break;

            case BinaryOp::NOT_EQ:
                result = leftVal != rightVal;
				break;

            case BinaryOp::AND:
                result = leftVal && rightVal;
				break;

            case BinaryOp::OR:
                result = leftVal || rightVal;
				break;

            default:
                result = -1; // FIXME: remove this
                // TODO: handle error
        }

		LOG("It's {}\n", result);

		return result;
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
        int operandVal = operand_->eval(ctx);

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
		MSG("Evaluating assignment\n");

        std::string destName = dest_->getName();

		MSG("Getting assigned value\n");
        int value = expr_->eval(ctx);
		LOG("Assigned value is {}\n", value);

        int32_t scopeId = 0;

        while (scopeId < ctx.curScope_)
		{
            if (ctx.varTables_[scopeId].contains(destName)) break;
			scopeId++;
		}

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

        while (cond_->eval(ctx))
        {
            result = scope_->eval(ctx);
        }

        return result;
    }
};

class IfNode final : public ConditionalStatementNode
{
private:
    ExprPtr cond_;
    StmtPtr action_;

public:
    IfNode(ExprPtr&& cond, StmtPtr&& action)
    :   cond_(std::move(cond)),
        action_(std::move(action)) {}

    int eval(detail::Context& ctx) const override
    {
        if (cond_->eval(ctx))
        {
            return action_->eval(ctx);
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
		MSG("Evaluation print\n");

        int value = expr_->eval(ctx);

        ctx.out << value;

        return value;
    }
};

class InNode final : public ExpressionNode
{
public:
    int eval([[maybe_unused]] detail::Context& ctx) const override
    {
        int value = 0;

        std::cin >> value;

        if (! std::cin.good())
        {
            // TODO: error handle
        }

        return value;
    }
};

class VoidNode final : public ExpressionNode
{
	int eval([[maybe_unused]] detail::Context& ctx) const override { return 0; }
};

} // namespace AST

#endif // ! NODE_HH

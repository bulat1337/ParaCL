#pragma once

#include "context.hh"
#include "log.hh"
#include "node.hh"
#include "visitor.hh"

namespace AST
{

namespace detail
{

class Interpreter final : public Visitor
{
  private:
    detail::Context ctx_;
    int buf_{};

  public:
    Interpreter(std::ostream &out = std::cout)
        : ctx_(out)
    {}

    int getBuf() const { return buf_; }

    void visit(const ConstantNode &node) override { buf_ = node.getVal(); }

    void visit(const VariableNode &node) override
    {
        std::string_view name = node.getName();
        LOG("Evaluating variable: {}\n", name);

        buf_ = ctx_.getVarValue(name);
    }

    void visit(const BinaryOpNode &node) override
    {
        MSG("Evaluating Binary Operation\n");

        node.accept_left(*this);
        int leftVal = buf_;

        node.accept_right(*this);
        int rightVal = buf_;

        int result = 0;

        switch (node.getOp())
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
                if (rightVal == 0)
                {
                    throw std::runtime_error("Divide by zero");
                }
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
                throw std::runtime_error("Unknown binary operation");
        }

        LOG("It's {}\n", result);

        buf_ = result;
    }

    void visit(const ScopeNode &node) override
    {
        if (node.empty())
            return;

        MSG("Evaluating scope\n");

        ++ctx_.curScope_;

        ctx_.varTables_.push_back(detail::Context::VarTable());

        LOG("ctx_.curScope_ = {}\n", ctx_.curScope_);
        LOG("ctx_.varTables_ size = {}\n", ctx_.varTables_.size());

        MSG("Scopes children:\n");
        for ([[maybe_unused]] const auto &child : node.getChildren())
        {
            LOG("{}\n", static_cast<const void *>(child));
        }

        for (const auto &child : node.getChildren())
        {
            LOG("Evaluating {}\n", static_cast<const void *>(child));
            child->accept(*this);
        }

        ctx_.varTables_.pop_back();

        --ctx_.curScope_;

        LOG("ctx_.curScope_ = {}\n", ctx_.curScope_);
        LOG("ctx_.varTables_ size = {}\n", ctx_.varTables_.size());
    }

    void visit(const UnaryOpNode &node) override
    {
        MSG("Evaluating Unary Operation\n");

        node.acceptOperand(*this);
        int operandVal = buf_;

        switch (node.getOp())
        {
            case UnaryOp::NEG:
                buf_ = -operandVal;
                break;

            case UnaryOp::NOT:
                buf_ = !operandVal;
                break;

            default:
                throw std::runtime_error("Unknown unary operation");
        }
    }

    void visit(const AssignNode &node) override
    {
        MSG("Evaluating assignment\n");

        MSG("Getting assigned value\n");

        node.acceptExpr(*this);
        int value = buf_;

        LOG("Assigned value is {}\n", value);

        std::string_view destName = node.getDestName();

        ctx_.get_variable(destName) = value;

        buf_ = value;
    }

    void visit(const WhileNode &node) override
    {
        // node.acceptCond(*this);
        // int cond = buf_;

        while (node.acceptCond(*this), buf_)
        {
            node.acceptScope(*this);
        }
    }

    void visit(const IfElseNode &node) override
    {
        if (!node.hasCond())
        {
            // if there is no condition do it
            node.acceptAction(*this);
            return;
        }

        node.acceptCond(*this);
        int cond = buf_;

        if (cond)
        {
            node.acceptAction(*this);
        }
        else
        {
            if (node.hasAltAction())
                node.acceptAltAction(*this);
        }
    }

    void visit(const PrintNode &node) override
    {
        MSG("Evaluation print\n");

        node.acceptExpr(*this);
        int value = buf_;

        ctx_.out << value << '\n';
    }

    void visit([[maybe_unused]] const InNode &node) override
    {
        int value = 0;

        std::cin >> value;

        if (!std::cin.good())
        {
            throw std::runtime_error("Incorrect input");
        }

        buf_ = value;
    }

    bool varInitialized(std::string_view varName) const
    {
        for (const auto &varTable : ctx_.varTables_)
        {
            if (varTable.count(varName) > 0)
                return true;
        }

        return false;
    }
};

}; // namespace detail

}; // namespace AST

#pragma once

#include "context.hh"
#include "log.hh"
#include "visitor.hh"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace AST
{

using VarIterator = std::unordered_map<std::string, int>::iterator;

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
  public:
    virtual void accept(detail::Visitor& visitor) const = 0;

    virtual ~INode() = default;
};

class StatementNode : public INode
{
};

class ExpressionNode : public StatementNode
{
};

class ConditionalStatementNode : public StatementNode
{
};

using ExprPtr = ExpressionNode*;

using StmtPtr = StatementNode*;

using CondStmtPtr = ConditionalStatementNode*;

class ScopeNode final : public StatementNode
{
  private:
    std::vector<StmtPtr> children_;

  public:
    ScopeNode(std::vector<StmtPtr>&& stms)
        : children_(std::move(stms))
    {}

    const std::vector<StmtPtr>& getChildren() const { return children_; }

    bool empty() const { return children_.empty(); }

    void pushChild(StmtPtr stmt) { children_.push_back(stmt); }

    size_t nstms() const { return children_.size(); }

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

using ScopePtr = ScopeNode*;

class ConstantNode final : public ExpressionNode
{
  private:
    const int val_;

  public:
    ConstantNode(int val)
        : val_(val)
    {}

    int getVal() const { return val_; }

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

class VariableNode final : public ExpressionNode
{
  private:
    std::string_view name_;

  public:
    VariableNode(std::string_view name)
        : name_(name)
    {}

    std::string_view getName() const { return name_; }

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

using VariablePtr = VariableNode*;

class BinaryOpNode final : public ExpressionNode
{
  private:
    ExprPtr left_{};
    ExprPtr right_{};
    BinaryOp op_{};

  public:
    void accept_left(detail::Visitor& visitor) const { left_->accept(visitor); }

    void accept_right(detail::Visitor& visitor) const
    {
        right_->accept(visitor);
    }

    BinaryOp getOp() const { return op_; }

    BinaryOpNode(ExprPtr left, BinaryOp op, ExprPtr right)
        : left_(left)
        , right_(right)
        , op_(op)
    {}

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

class UnaryOpNode final : public ExpressionNode
{
  private:
    ExprPtr operand_{};
    UnaryOp op_{};

  public:
    UnaryOpNode(ExprPtr operand, UnaryOp op)
        : operand_(operand)
        , op_(op)
    {}

    void acceptOperand(detail::Visitor& visitor) const
    {
        operand_->accept(visitor);
    }

    UnaryOp getOp() const { return op_; }

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

class RepeatNode;
using RepeatPtr = RepeatNode*;

class ArrayInitNode : public StatementNode
{
  private:
    std::vector<ExprPtr> init_list_;

  public:
    ArrayInitNode(std::vector<ExprPtr>&& init_list)
        : init_list_(std::move(init_list))
    {}

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }

    size_t arraySize() const 
    {
        return init_list_.size();
    }

    void acceptElem(size_t index, detail::Visitor& visitor) const
    {
        init_list_[index]->accept(visitor);
    }
};

using ArrayInitPtr = ArrayInitNode*;


using Rhs = std::variant<ExprPtr, RepeatPtr, ArrayInitPtr>;

class RepeatNode final : public StatementNode
{
  private:
    Rhs elem_{};
    ExprPtr size_{};

  public:
    RepeatNode(ExprPtr size)
        : size_(size)
    {}

    RepeatNode(Rhs elem, ExprPtr size)
        : elem_(elem)
        , size_(size)
    {}

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }

    void acceptSize(detail::Visitor& visitor) const { size_->accept(visitor); }

    void acceptElem(detail::Visitor& visitor) const
    {
        std::visit(
            [&](auto&& elem)
            {
                if (!elem)
                    throw std::runtime_error("No defined element value\n");

                elem->accept(visitor);
            },
            elem_);
    }

    bool hasElem() const
    {
        return std::visit([&](auto&& elem) { return elem != nullptr; }, elem_);
    }
};

class ArrayElemNode;

using ArrayElemPtr = ArrayElemNode*;

using Lhs = std::variant<VariablePtr, ArrayElemPtr>;

class ArrayElemNode final : public ExpressionNode
{
  private:
    Lhs name_{};
    ExprPtr index_{};

  public:
    ArrayElemNode(Lhs name, ExprPtr index)
        : name_(name)
        , index_(index)
    {}

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }

    void acceptIndex(detail::Visitor& visitor) const
    {
        MSG("Getting index value\n");
        index_->accept(visitor);
    }

    void acceptName(detail::Visitor& visitor) const
    {
        MSG("Accepting ArrayElem name\n");

        if (auto name = std::get_if<ArrayElemPtr>(&name_))
            return (*name)->accept(visitor);

        throw std::runtime_error("Can't accept ArrayElem name\n");
    }

    std::string_view getName() const
    {
        if (auto var = std::get_if<VariablePtr>(&name_))
            return (*var)->getName();

        throw std::runtime_error("Can't get name of arrayElem\n");
    }

    bool holdsVariable() const
    {
        return std::holds_alternative<VariablePtr>(name_);
    }

    bool holdsArrayElem() const
    {
        return std::holds_alternative<ArrayElemPtr>(name_);
    }
};

class AssignNode final : public ExpressionNode
{
  private:
    Lhs dest_{};
    Rhs src_{};

  public:
    AssignNode(Lhs dest, Rhs src)
        : dest_(dest)
        , src_(src)
    {}

    void acceptSrc(detail::Visitor& visitor) const
    {
        std::visit([&visitor](auto&& node) { node->accept(visitor); }, src_);
    }

    std::string_view getDestName() const
    {
        return std::visit(
            [](const auto& dest) -> std::string_view
            {
                if constexpr (requires { dest->getName(); })
                {
                    return dest->getName();
                }
                else
                    throw std::runtime_error(
                        "Can't get a name of an Array Element\n");
            },
            dest_);
    }

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }

    const Rhs& getSrc() const { return src_; }

    const Lhs& getDest() const { return dest_; }
};

class WhileNode final : public ConditionalStatementNode
{
  private:
    ExprPtr cond_{};
    StmtPtr scope_{};

  public:
    WhileNode(ExprPtr cond, StmtPtr scope)
        : cond_(cond)
        , scope_(scope)
    {}

    void acceptCond(detail::Visitor& visitor) const { cond_->accept(visitor); }

    void acceptScope(detail::Visitor& visitor) const
    {
        scope_->accept(visitor);
    }

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

class IfElseNode final : public StatementNode
{
  private:
    ExprPtr cond_{};
    StmtPtr action_{};
    StmtPtr alt_action_{};

  public:
    IfElseNode(ExprPtr cond, StmtPtr action)
        : cond_(cond)
        , action_(action)
    {}

    IfElseNode(ExprPtr cond, StmtPtr action, StmtPtr alt_action)
        : cond_(cond)
        , action_(action)
        , alt_action_(alt_action)
    {}

    IfElseNode(StmtPtr action)
        : action_(action)
    {}

    bool hasCond() const { return cond_ != nullptr; }

    bool hasAltAction() const { return alt_action_ != nullptr; }

    void acceptCond(detail::Visitor& visitor) const { cond_->accept(visitor); }

    void acceptAction(detail::Visitor& visitor) const
    {
        action_->accept(visitor);
    }

    void acceptAltAction(detail::Visitor& visitor) const
    {
        alt_action_->accept(visitor);
    }

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

class PrintNode final : public StatementNode
{
  private:
    ExprPtr expr_{};

  public:
    PrintNode(ExprPtr expr)
        : expr_(expr)
    {}

    void acceptExpr(detail::Visitor& visitor) const { expr_->accept(visitor); }

    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

class InNode final : public ExpressionNode
{
  public:
    void accept(detail::Visitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

} // namespace AST

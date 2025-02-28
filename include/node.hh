#ifndef NODE_HH
#define NODE_HH

#include "context.hh"
#include "log.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
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

class ConstantNode;
class VariableNode;
class BinaryOpNode;
class ScopeNode;
class UnaryOpNode;
class AssignNode;
class WhileNode;
class IfElseNode;
class PrintNode;
class InNode;

class Visitor
{
  public:
	virtual void visit(const ConstantNode& node) = 0;
	virtual void visit(const VariableNode& node) = 0;
	virtual void visit(const BinaryOpNode& node) = 0;
	virtual void visit(const ScopeNode& node) = 0;
	virtual void visit(const UnaryOpNode& node) = 0;
	virtual void visit(const AssignNode& node) = 0;
	virtual void visit(const WhileNode& node) = 0;
	virtual void visit(const IfElseNode& node) = 0;
	virtual void visit(const PrintNode& node) = 0;
	virtual void visit(const InNode& node) = 0;

	virtual ~Visitor() = default;
};

class INode
{
  public:
	virtual void accept(Visitor& visitor) const = 0;

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

using ExprPtr = ExpressionNode *;

using StmtPtr = StatementNode *;

using CondStmtPtr = ConditionalStatementNode *;

class ScopeNode final : public StatementNode
{
  private:
    std::vector<StmtPtr> children_;

  public:
    ScopeNode(std::vector<StmtPtr> &&stms)
        : children_(std::move(stms))
    {}

	const std::vector<StmtPtr> getChildren() const
	{
		return children_;
	}

	bool empty() const { return children_.empty(); }

    void pushChild(StmtPtr stmt) { children_.push_back(stmt); }

    size_t nstms() const { return children_.size(); }

	void accept(Visitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

using ScopePtr = ScopeNode *;

class ConstantNode final : public ExpressionNode
{
  private:
    const int val_;

  public:
    ConstantNode(int val)
        : val_(val)
    {}

    int getVal() const { return val_; }

	void accept(Visitor& visitor) const override
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

	void accept(Visitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

using VariablePtr = VariableNode *;

class BinaryOpNode final : public ExpressionNode
{
  private:
    ExprPtr left_{};
    ExprPtr right_{};
    BinaryOp op_{};

  public:
	void accept_left(Visitor& visitor) const
	{
		left_->accept(visitor);
	}

	void accept_right(Visitor& visitor) const
	{
		right_->accept(visitor);
	}

	BinaryOp getOp() const { return op_; }

    BinaryOpNode(ExprPtr left, BinaryOp op, ExprPtr right)
        : left_(left)
        , right_(right)
        , op_(op)
    {}

	void accept(Visitor& visitor) const override
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

	void acceptOperand(Visitor& visitor) const
	{
		operand_->accept(visitor);
	}

	UnaryOp getOp() const
	{
		return op_;
	}

	void accept(Visitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

class AssignNode final : public ExpressionNode
{
  private:
    VariablePtr dest_{};
    ExprPtr expr_{};

  public:
    AssignNode(VariablePtr dest, ExprPtr expr)
        : dest_(dest)
        , expr_(expr)
    {}

	void acceptExpr(Visitor& visitor) const
	{
		expr_->accept(visitor);
	}

	std::string_view getDestName() const
	{
		return dest_->getName();
	}

	void accept(Visitor& visitor) const override
	{
		visitor.visit(*this);
	}
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

	void acceptCond(Visitor& visitor) const
	{
		cond_->accept(visitor);
	}

	void acceptScope(Visitor& visitor) const
	{
		scope_->accept(visitor);
	}

	void accept(Visitor& visitor) const override
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

	bool hasCond() const
	{
		return cond_ != nullptr;
	}

	bool hasAltAction() const
	{
		return alt_action_ != nullptr;
	}

	void acceptCond(Visitor& visitor) const
	{
		cond_->accept(visitor);
	}

	void acceptAction(Visitor& visitor) const
	{
		action_->accept(visitor);
	}

	void acceptAltAction(Visitor& visitor) const
	{
		alt_action_->accept(visitor);
	}

	void accept(Visitor& visitor) const override
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

	void acceptExpr(Visitor& visitor) const
	{
		expr_->accept(visitor);
	}

	void accept(Visitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

class InNode final : public ExpressionNode
{
  public:
	void accept(Visitor& visitor) const override
	{
		visitor.visit(*this);
	}
};

} // namespace AST

#endif // ! NODE_HH

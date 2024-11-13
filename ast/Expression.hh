#pragma once

#include <memory>
#include "ASTNode.hh"
#include <string>

using ExprPtr = std::unique_ptr<class Expression>;
using IntLiteralPtr = std::unique_ptr<class IntLiteral>;
using BinaryExprPtr = std::unique_ptr<class BinaryExpr>;

enum class BinaryOp { ADD, SUBTRACT, MULTIPLY, DIVIDE };

class Expression : public INode {};

class IntLiteral : public Expression
{
public:
    IntLiteral(int value) 
    :   value_(value) {}

    void accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }

    std::string toString() const override { return std::to_string(value_); }

    int getValue() const { return value_; }

private:
    int value_;
};

class VariableExpr : public Expression
{
public:
    VariableExpr(const std::string& name) 
    :   name_(name) {}

    void accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }

    std::string toString() const override { return name_; }

private:
    std::string name_;
};

class BinaryExpr : public Expression
{
public:
    BinaryExpr(BinaryExprPtr lhs, BinaryOp op, BinaryExprPtr rhs)
    :   lhs_(lhs),
        op_(op),
        rhs_(rhs) {}

    void accept(Visitor& visitor) override
    {
        return visitor.visit(*this);
    }

    std::string toString() const override 
    {
        return "(" + lhs_->toString() + " " +
               (op_ == BinaryOp::ADD ? "+" :
                op_ == BinaryOp::SUBTRACT ? "-" :
                op_ == BinaryOp::MULTIPLY ? "*" : "/") +
               " " + rhs_->toString() + ")";
    }

private:
    BinaryExprPtr lhs_;
    BinaryOp op_;
    BinaryExprPtr rhs_;
};

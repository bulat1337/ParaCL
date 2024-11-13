#pragma once

#include "ASTNode.hh"
#include "Expression.hh"
#include "PrintVisitor.hh"
#include <memory>
#include <vector>

class Statement : public INode {};
class BlockStmt;

using StmtPtr = std::unique_ptr<Statement>;
using BlockStmtPtr = std::unique_ptr<BlockStmt>;

class VarDecl : public Statement
{
public:
    VarDecl(const std::string& name)
    :   name_(name) {}

    void accept(PrintVisitor& visitor) override
    {
        return visitor.visit(*this);
    }

    std::string toString() const override { return "int " + name_ + ";"; }

private:
    std::string name_;
};

class AssignStmt : public Statement
{
public:
    AssignStmt(std::string name, ExprPtr expr)
    :   name_(name),
        expr_(std::move(expr)) {}

    void accept(PrintVisitor& visitor) override
    {
        return visitor.visit(*this);
    }

    std::string toString() const override 
    {
        return name_ + " = " + expr_->toString() + ";";
    }

private:
    std::string name_;
    ExprPtr expr_;
};

class IfStmt : public Statement
{
public:
    IfStmt(ExprPtr cond, BlockStmtPtr block)
    :   cond_(std::move(cond)),
        block_(std::move(block)) {}

    void accept(PrintVisitor& visitor) override
    {
        return visitor.visit(*this);
    }

    std::string toString() const override 
    {
        std::string result = "if (" + cond_->toString() + ") " + block_->toString();

        return result;
    }

private:
    ExprPtr cond_;
    BlockStmtPtr block_;
};

class WhileStmt : public Statement
{
public:
    WhileStmt(ExprPtr cond, BlockStmtPtr block)
    :   cond_(std::move(cond)),
        block_(std::move(block)) {}

    void accept(PrintVisitor& visitor) override
    {
        return visitor.visit(*this);
    }

    std::string toString() const override 
    {
        return "while (" + cond_->toString() + ") " + block_->toString();
    }

private:
    ExprPtr cond_;
    BlockStmtPtr block_;
};

class BlockStmt : public Statement
{
public:
    BlockStmt(std::vector<StmtPtr> statements)
    :   statements_(std::move(statements)) {}
        
    void accept(PrintVisitor& visitor) override
    {
        return visitor.visit(*this);
    }

    std::string toString() const override 
    {
        std::string result = "{\n";
        for (const auto& stmt : statements_)
        {
            result += "  " + stmt->toString() + "\n";
        }

        return result + "}";
    }

private:
    std::vector<StmtPtr> statements_;
};



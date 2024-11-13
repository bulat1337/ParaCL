#pragma once

class IntLiteral;
class VariableExpr;
class BinaryExpr;
class VarDecl;
class AssignStmt;
class IfStmt;
class WhileStmt;
class BlockStmt;

class Visitor 
{
  public:
    virtual ~Visitor() = default;

    // Visit methods for each AST node 
    virtual void visit(IntLiteral& node)   = 0;
    virtual void visit(VariableExpr& node) = 0;
    virtual void visit(BinaryExpr& node)   = 0;
    virtual void visit(VarDecl& node)      = 0;
    virtual void visit(AssignStmt& node)   = 0;
    virtual void visit(IfStmt& node)       = 0;
    virtual void visit(WhileStmt& node)    = 0;
    virtual void visit(BlockStmt& node)    = 0;
};



#pragma once

namespace AST
{

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

namespace detail
{

class Visitor
{
  public:
    virtual void visit(const ConstantNode &node) = 0;
    virtual void visit(const VariableNode &node) = 0;
    virtual void visit(const BinaryOpNode &node) = 0;
    virtual void visit(const ScopeNode &node) = 0;
    virtual void visit(const UnaryOpNode &node) = 0;
    virtual void visit(const AssignNode &node) = 0;
    virtual void visit(const WhileNode &node) = 0;
    virtual void visit(const IfElseNode &node) = 0;
    virtual void visit(const PrintNode &node) = 0;
    virtual void visit(const InNode &node) = 0;

    virtual ~Visitor() = default;
};

}; // namespace detail

}; // namespace AST

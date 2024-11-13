#include <iostream>
#include "Visitor.hh"
#include "ASTNode.hh"

class PrintVisitor : public Visitor {
public:
    void visit(IntLiteral& node) override {
        std::cout << "IntLiteral: " << node.toString() << std::endl;
    }

    void visit(VariableExpr& node) override {
        std::cout << "VariableExpr: " << node.toString() << std::endl;
    }

    void visit(BinaryExpr& node) override {
        std::cout << "BinaryExpr: " << node.toString() << std::endl;
    }

    void visit(VarDecl& node) override {
        std::cout << "VarDecl: " << node.toString() << std::endl;
    }

    void visit(AssignStmt& node) override {
        std::cout << "AssignStmt: " << node.toString() << std::endl;
    }

    void visit(IfStmt& node) override {
        std::cout << "IfStmt: " << node.toString() << std::endl;
    }

    void visit(WhileStmt& node) override {
        std::cout << "WhileStmt: " << node.toString() << std::endl;
    }

    void visit(BlockStmt& node) override {
        std::cout << "BlockStmt: " << node.toString() << std::endl;
    }
};



#include "Statement.hh"
#include "Expression.hh"
#include <iostream>

int main() {
    // Build an AST for: int x; x = 5; while (x > 0) { x = x - 1; }
    auto program = std::make_unique<BlockStmt>(
        std::vector<std::unique_ptr<Statement>>{
            std::make_unique<VarDecl>("x"),
            std::make_unique<AssignStmt>("x", std::make_unique<IntLiteral>(5)),
            std::make_unique<WhileStmt>(
                std::make_unique<BinaryExpr>(
                    std::make_unique<VariableExpr>("x"), BinaryOp::SUBTRACT, std::make_unique<IntLiteral>(1)
                ),
                std::make_unique<AssignStmt>("x", std::make_unique<BinaryExpr>(
                    std::make_unique<VariableExpr>("x"), BinaryOp::SUBTRACT, std::make_unique<IntLiteral>(1)
                ))
            )
        }
    );

    std::cout << program->toString() << std::endl;
    return 0;
}


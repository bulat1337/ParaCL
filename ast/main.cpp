#include "INode.hh"
#include "Node.hh"
#include <iostream>
#include <memory>

namespace AST {

void testScopes() {
    // Create the global scope (root scope)
    auto globalScope = std::make_unique<Scope>(nullptr);

    // Insert variables into the global scope
    globalScope->insertVariable("x", 10);
    globalScope->insertVariable("y", 20);
    globalScope->insertVariable("z", 30);

    // Create a nested scope (child of the global scope)
    auto childScope = std::make_unique<Scope>(globalScope.get());

    // Insert variables into the child scope
    childScope->insertVariable("a", 40);
    childScope->insertVariable("b", 50);

    // Create another nested scope (child of the child scope)
    auto grandchildScope = std::make_unique<Scope>(childScope.get());

    // Insert variables into the grandchild scope
    grandchildScope->insertVariable("m", 60);
    grandchildScope->insertVariable("n", 70);

    // Check the variables at different levels of the scope hierarchy
    std::cout << "Global Scope Variables:\n";
    std::cout << "x = " << globalScope->getVariableIterator("x")->second << "\n";
    std::cout << "y = " << globalScope->getVariableIterator("y")->second << "\n";
    std::cout << "z = " << globalScope->getVariableIterator("z")->second << "\n";

    std::cout << "\nChild Scope Variables:\n";
    std::cout << "a = " << childScope->getVariableIterator("a")->second << "\n";
    std::cout << "b = " << childScope->getVariableIterator("b")->second << "\n";
    std::cout << "x = " << childScope->getVariableIterator("x")->second << "\n";  // Access global variable from child scope

    std::cout << "\nGrandchild Scope Variables:\n";
    std::cout << "m = " << grandchildScope->getVariableIterator("m")->second << "\n";
    std::cout << "n = " << grandchildScope->getVariableIterator("n")->second << "\n";
    std::cout << "a = " << grandchildScope->getVariableIterator("a")->second << "\n";  // Access child variable from grandchild scope
}

} // namespace AST

int main() {

    auto globalScope = std::make_unique<AST::Scope>(nullptr);

    globalScope->insertVariable("x", 10);
    
    std::unique_ptr<AST::INode> sum = std::make_unique<AST::BinaryOpNode>(std::make_unique<AST::ConstantNode>(10), AST::BinaryOp::ADD, std::make_unique<AST::ConstantNode>(11));

    auto x = std::make_unique<AST::VariableNode>(globalScope->getVariableIterator("x"));
    auto assign = std::make_unique<AST::AssignNode>(std::move(x), std::move(sum));


    std::cout << "x = " << assign->eval() << "\n";
    // AST::testScopes();
    return 0;
}


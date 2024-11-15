#include "Node.hh"
#include <iostream>

int main()
{
    auto globalScope = std::make_unique<AST::Scope>(nullptr);

    // Insert a variable into the scope
    globalScope->insertVariable("x");

    // Get an iterator to the variable "x"
    auto varIt = globalScope->getVariableIterator("x");

    // Create a VariableNode using the iterator
    std::unique_ptr<AST::VariableNode> varNode =
        std::make_unique<AST::VariableNode>(varIt);
    std::unique_ptr<AST::INode> constNode =
        std::make_unique<AST::ConstantNode>(42);
    auto assignNode = std::make_unique<AST::AssignNode>(std::move(varNode),
                                                        std::move(constNode));

    // Evaluate the assignment
    assignNode->eval();

    // Check the updated value
    std::cout << "x = " << varIt->second << std::endl; // Should print: x = 42

    return 0;
}

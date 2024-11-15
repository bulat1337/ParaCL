#include "node.hh"
#include "test_utils.h"

#include <gtest/gtest.h> // for Test, TestInfo (ptr only), Message, TEST
#include <memory>

TEST(common, basic_1) { test_utils::run_test("/common/basic_1"); }

TEST(common, local_variables)
{
    test_utils::run_test("/common/local_variables");
}

TEST(AST, assign)
{
    auto globalScope = std::make_unique<AST::Scope>(nullptr);

    globalScope->insertVariable("x");

    auto x = std::make_unique<AST::VariableNode>(
        globalScope->getVariableIterator("x"));

    auto constant = std::make_unique<AST::ConstantNode>(1000);

    auto assign =
        std::make_unique<AST::AssignNode>(std::move(x), std::move(constant));

    EXPECT_EQ(assign->eval(), 1000);
}

TEST(AST, checkAddition)
{
    auto globalScope = std::make_unique<AST::Scope>(nullptr);

    globalScope->insertVariable("x");

    std::unique_ptr<AST::INode> sum = std::make_unique<AST::BinaryOpNode>(
        std::make_unique<AST::ConstantNode>(1000), AST::BinaryOp::ADD,
        std::make_unique<AST::ConstantNode>(7));

    auto x = std::make_unique<AST::VariableNode>(
        globalScope->getVariableIterator("x"));
    auto assign =
        std::make_unique<AST::AssignNode>(std::move(x), std::move(sum));

    EXPECT_EQ(assign->eval(), 1007);
}

TEST(AST, checkSub)
{
    auto globalScope = std::make_unique<AST::Scope>(nullptr);

    globalScope->insertVariable("x");

    std::unique_ptr<AST::INode> sum = std::make_unique<AST::BinaryOpNode>(
        std::make_unique<AST::ConstantNode>(1000), AST::BinaryOp::SUB,
        std::make_unique<AST::ConstantNode>(7));

    auto x = std::make_unique<AST::VariableNode>(
        globalScope->getVariableIterator("x"));
    auto assign =
        std::make_unique<AST::AssignNode>(std::move(x), std::move(sum));

    EXPECT_EQ(assign->eval(), 993);
}

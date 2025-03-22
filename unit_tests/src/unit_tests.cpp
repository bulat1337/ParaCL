#include <gtest/gtest.h>   // for Test, Message, TestInfo (ptr only), CmpHel...
#include <sstream>         // for basic_stringstream, basic_iostream, basic_...
#include <stdexcept>       // for runtime_error
#include <string>          // for basic_string, allocator, char_traits
#include <string_view>     // for basic_string_view
#include <vector>          // for vector

#include "ast.hh"          // for AST
#include "driver.hh"       // for Driver
#include "interpreter.hh"  // for Interpreter
#include "node.hh"         // for ConstantNode, BinaryOpNode, AssignNode
#include "test_utils.hh"   // for run_test

TEST(common, basic_1) { test_utils::run_test("/common/basic_1"); }

TEST(common, local_variables)
{
    test_utils::run_test("/common/local_variables");
}

TEST(common, if_1) { test_utils::run_test("/common/if_1"); }

TEST(common, if_2) { test_utils::run_test("/common/if_2"); }

TEST(common, while_1) { test_utils::run_test("/common/while_1"); }

TEST(common, while_uminus_logical)
{
    test_utils::run_test("/common/while_uminus_logical");
}

TEST(common, while_if_logical_mod)
{
    test_utils::run_test("/common/while_if_logical_mod");
}

TEST(common, expression_1) { test_utils::run_test("/common/expression_1"); }

TEST(common, scopes) { test_utils::run_test("/common/scopes"); }

TEST(common, comments) { test_utils::run_test("/common/comments"); }

TEST(common, blank_file) { test_utils::run_test("/common/blank_file"); }

TEST(common, if_else) { test_utils::run_test("/common/if_else"); }

TEST(common, else_if_1) { test_utils::run_test("/common/else_if_1"); }

TEST(common, else_if_2) { test_utils::run_test("/common/else_if_2"); }

TEST(common, associative_assign)
{
    test_utils::run_test("/common/associative_assign");
}

TEST(common, multiple_negate)
{
    test_utils::run_test("/common/multiple_negate");
}

TEST(common, assign_and_logical)
{
    test_utils::run_test("/common/assign_and_logical");
}

TEST(common, logical_negation)
{
    test_utils::run_test("/common/logical_negation");
}

TEST(common, logical_print_1)
{
    test_utils::run_test("/common/logical_print_1");
}

TEST(common, logical_negation_simple)
{
    test_utils::run_test("/common/logical_negation_simple");
}

TEST(common, mod_simple) { test_utils::run_test("/common/mod_simple"); }

TEST(common, comp_simple) { test_utils::run_test("/common/comp_simple"); }

TEST(common, mod) { test_utils::run_test("/common/mod"); }

TEST(common, array_basic_1) { test_utils::run_test("/common/array_basic_1"); }

TEST(common, array_undef) { test_utils::run_test("/common/array_undef"); }

TEST(common, array_multidim) { test_utils::run_test("/common/array_multidim"); }

TEST(common, array_repeat_in_repeat) { test_utils::run_test("/common/array_repeat_in_repeat"); }

TEST(ASTTest, CreateConstant)
{
    AST::AST ast;
    const auto constNode = ast.construct<AST::ConstantNode>(42);

    AST::detail::Interpreter interpreter;

    ASSERT_NE(constNode, nullptr);
    interpreter.visit(*constNode);
    EXPECT_EQ(interpreter.getBuf(), 42);
}

TEST(ASTTest, CreateVariable)
{
    AST::AST ast;
    auto varNode = ast.construct<AST::VariableNode>("x");
    ASSERT_NE(varNode, nullptr);
    EXPECT_EQ(varNode->getName(), "x");
}

TEST(ASTTest, CreateBinaryOpADD)
{
    AST::AST ast;
    const auto lhs = ast.construct<AST::ConstantNode>(1000);
    const auto rhs = ast.construct<AST::ConstantNode>(7);
    const auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::ADD, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);

    EXPECT_EQ(interpreter.getBuf(), 1007);
}

TEST(ASTTest, CreateBinaryOpSUB)
{
    AST::AST ast;
    const auto lhs = ast.construct<AST::ConstantNode>(1000);
    const auto rhs = ast.construct<AST::ConstantNode>(7);
    const auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::SUB, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 993);
}

TEST(ASTTest, CreateBinaryOpMUL)
{
    AST::AST ast;
    const auto lhs = ast.construct<AST::ConstantNode>(15);
    const auto rhs = ast.construct<AST::ConstantNode>(3);
    const auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::MUL, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 45);
}

TEST(ASTTest, CreateBinaryOpDIV)
{
    AST::AST ast;
    const auto lhs = ast.construct<AST::ConstantNode>(42);
    const auto rhs = ast.construct<AST::ConstantNode>(6);
    const auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::DIV, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 7);
}

TEST(ASTTest, CreateBinaryOpDIVByZero)
{
    AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(42);
    auto rhs = ast.construct<AST::ConstantNode>(0);
    auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::DIV, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    EXPECT_THROW(interpreter.visit(*binOpNode), std::runtime_error);
}

TEST(ASTTest, CreateBinaryOpMOD)
{
    AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(10);
    auto rhs = ast.construct<AST::ConstantNode>(3);
    auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::MOD, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 1);
}

TEST(ASTTest, CreateBinaryOpAND)
{
    AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(1);
    auto rhs = ast.construct<AST::ConstantNode>(0);
    auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::AND, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);

    EXPECT_EQ(interpreter.getBuf(), 0);
}

TEST(ASTTest, CreateBinaryOpOR)
{
    AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(1);
    auto rhs = ast.construct<AST::ConstantNode>(0);
    auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::OR, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 1);
}

TEST(ASTTest, CreateBinaryOpLESS)
{
    AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(3);
    auto rhs = ast.construct<AST::ConstantNode>(7);
    auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::LS, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 1);
}

TEST(ASTTest, CreateBinaryOpLESSorEQUAL)
{
    AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(3);
    auto rhs = ast.construct<AST::ConstantNode>(7);
    auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::LS_EQ, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 1);
}

TEST(ASTTest, CreateBinaryOpGREATER)
{
    AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(10);
    auto rhs = ast.construct<AST::ConstantNode>(5);
    auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::GR, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 1);
}

TEST(ASTTest, CreateBinaryOpGREATERorEQUAL)
{
    AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(5);
    auto rhs = ast.construct<AST::ConstantNode>(5);
    auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::GR_EQ, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 1);
}

TEST(ASTTest, CreateBinaryOpEQ)
{
    AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(42);
    auto rhs = ast.construct<AST::ConstantNode>(42);
    auto binOpNode =
        ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::EQ, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*binOpNode);
    EXPECT_EQ(interpreter.getBuf(), 1);
}

TEST(ASTTest, CreateUnaryOpNEG)
{
    AST::AST ast;
    auto operand = ast.construct<AST::ConstantNode>(-42);
    auto unaryOpNode =
        ast.construct<AST::UnaryOpNode>(operand, AST::UnaryOp::NEG);

    ASSERT_NE(unaryOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*unaryOpNode);
    EXPECT_EQ(interpreter.getBuf(), 42);
}

TEST(ASTTest, CreateUnaryOpNOT)
{
    AST::AST ast;
    auto operand = ast.construct<AST::ConstantNode>(0);
    auto unaryOpNode =
        ast.construct<AST::UnaryOpNode>(operand, AST::UnaryOp::NOT);

    ASSERT_NE(unaryOpNode, nullptr);

    AST::detail::Interpreter interpreter;
    interpreter.visit(*unaryOpNode);
    EXPECT_EQ(interpreter.getBuf(), 1);
}

TEST(ASTTest, CreateAssignmentNode)
{
    Driver drv;
    const auto var = drv.construct<AST::VariableNode>("x");
    const auto expr = drv.construct<AST::ConstantNode>(42);
    const auto assignmentNode = drv.construct<AST::AssignNode>(var, expr);

    ASSERT_NE(assignmentNode, nullptr);

    drv.curScope().push_back(assignmentNode);
    drv.formGlobalScope();
    drv.eval();

    EXPECT_EQ(drv.getInterpreterBuf(), 42);
}

TEST(ASTTest, WhileNode_ConditionTrue)
{
    Driver drv;

    const auto var = drv.construct<AST::VariableNode>("x");
    const auto expr = drv.construct<AST::ConstantNode>(0);
    const auto assignmentNode = drv.construct<AST::AssignNode>(var, expr);

    ASSERT_NE(assignmentNode, nullptr);

    drv.curScope().push_back(assignmentNode);

    const auto condition = drv.construct<AST::BinaryOpNode>(
        drv.construct<AST::VariableNode>("x"), AST::BinaryOp::LS,
        drv.construct<AST::ConstantNode>(10));
    const auto action = drv.construct<AST::AssignNode>(
        drv.construct<AST::VariableNode>("x"),
        drv.construct<AST::BinaryOpNode>(drv.construct<AST::VariableNode>("x"),
                                         AST::BinaryOp::ADD,
                                         drv.construct<AST::ConstantNode>(1)));
    const auto whileNode = drv.construct<AST::WhileNode>(condition, action);

    ASSERT_NE(whileNode, nullptr);

    drv.curScope().push_back(whileNode);
    drv.formGlobalScope();
    drv.eval();

    EXPECT_EQ(drv.getInterpreterBuf(), 0);
}

TEST(ASTTest, IfNode_TrueCondition)
{
    Driver drv;

    const auto var = drv.construct<AST::VariableNode>("x");
    const auto expr = drv.construct<AST::ConstantNode>(10);
    const auto assignmentNode = drv.construct<AST::AssignNode>(var, expr);

    ASSERT_NE(assignmentNode, nullptr);

    drv.curScope().push_back(assignmentNode);

    const auto condition = drv.construct<AST::BinaryOpNode>(
        drv.construct<AST::VariableNode>("x"), AST::BinaryOp::EQ,
        drv.construct<AST::ConstantNode>(10));
    const auto action =
        drv.construct<AST::AssignNode>(drv.construct<AST::VariableNode>("y"),
                                       drv.construct<AST::ConstantNode>(20));

    const auto ifNode = drv.construct<AST::IfElseNode>(condition, action);

    ASSERT_NE(ifNode, nullptr);

    drv.curScope().push_back(ifNode);
    drv.formGlobalScope();
    drv.eval();

    EXPECT_EQ(drv.getInterpreterBuf(), 20);
}
//
TEST(ASTTest, IfNode_FalseCondition)
{
    Driver drv;

    auto condition = drv.construct<AST::BinaryOpNode>(
        drv.construct<AST::ConstantNode>(9), AST::BinaryOp::EQ,
        drv.construct<AST::ConstantNode>(10));
    auto action =
        drv.construct<AST::AssignNode>(drv.construct<AST::VariableNode>("y"),
                                       drv.construct<AST::ConstantNode>(20));
    auto ifNode = drv.construct<AST::IfElseNode>(condition, action);

    ASSERT_NE(ifNode, nullptr);

    drv.curScope().push_back(ifNode);
    drv.formGlobalScope();
    drv.eval();

    EXPECT_EQ(drv.varInitialized("y"), false);
}

TEST(ASTTest, PrintNode)
{
    std::stringstream out;

    Driver drv(out);

    const auto var = drv.construct<AST::VariableNode>("x");
    const auto expr = drv.construct<AST::ConstantNode>(42);
    const auto assignmentNode = drv.construct<AST::AssignNode>(var, expr);

    ASSERT_NE(assignmentNode, nullptr);

    drv.curScope().push_back(assignmentNode);

    auto printNode =
        drv.construct<AST::PrintNode>(drv.construct<AST::VariableNode>("x"));

    ASSERT_NE(printNode, nullptr);

    drv.curScope().push_back(printNode);
    drv.formGlobalScope();
    drv.eval();

    EXPECT_EQ(out.str(), "42\n");
}

#include <gtest/gtest.h>  // for Test, TestInfo (ptr only), TEST
#include <string>         // for basic_string
#include <vector>
#include "dsl.hh"
#include "node.hh"
#include "ast.hh"

#include "test_utils.h"   // for run_test

TEST(common, basic_1) { test_utils::run_test("/common/basic_1"); }

TEST(common, local_variables) { test_utils::run_test("/common/local_variables"); }

TEST(common, if_1) { test_utils::run_test("/common/if_1"); }

TEST(common, if_2) { test_utils::run_test("/common/if_2"); }

TEST(common, while_1) { test_utils::run_test("/common/while_1"); }

TEST(common, while_uminus_logical) { test_utils::run_test("/common/while_uminus_logical"); }

TEST(common, while_if_logical_mod) { test_utils::run_test("/common/while_if_logical_mod"); }

TEST(common, expression_1) { test_utils::run_test("/common/expression_1"); }

TEST(common, scopes) { test_utils::run_test("/common/scopes"); }

TEST(common, comments) { test_utils::run_test("/common/comments"); }

TEST(common, blank_file) { test_utils::run_test("/common/blank_file"); }

TEST(common, if_else) { test_utils::run_test("/common/if_else"); }

TEST(common, else_if_1) { test_utils::run_test("/common/else_if_1"); }

TEST(common, else_if_2) { test_utils::run_test("/common/else_if_2"); }

TEST(ASTTest, CreateConstant)
{
    auto constNode = MAKE_CONST(42);
    ASSERT_NE(constNode, nullptr);
    EXPECT_EQ(constNode->getVal(), 42);
}

TEST(ASTTest, CreateVariable)
{
    auto varNode = MAKE_VAR("x");
    ASSERT_NE(varNode, nullptr);
    EXPECT_EQ(varNode->getName(), "x");
}

TEST(ASTTest, CreateBinaryOpADD)
{
    auto lhs = MAKE_CONST(1000);
    auto rhs = MAKE_CONST(7);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::ADD, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);

    EXPECT_EQ(result, 1007);
}

TEST(ASTTest, CreateBinaryOpSUB)
{
    auto lhs = MAKE_CONST(1000);
    auto rhs = MAKE_CONST(7);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::SUB, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 993);
}

TEST(ASTTest, CreateBinaryOpMUL)
{
    auto lhs = MAKE_CONST(15);
    auto rhs = MAKE_CONST(3);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::MUL, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 45);
}

TEST(ASTTest, CreateBinaryOpDIV)
{
    auto lhs = MAKE_CONST(42);
    auto rhs = MAKE_CONST(6);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::DIV, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 7);
}

TEST(ASTTest, CreateBinaryOpDIVByZero)
{
    auto lhs = MAKE_CONST(42);
    auto rhs = MAKE_CONST(0);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::DIV, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    EXPECT_THROW(binOpNode->eval_value(ctx), std::runtime_error);
}

TEST(ASTTest, CreateBinaryOpMOD)
{
    auto lhs = MAKE_CONST(10);
    auto rhs = MAKE_CONST(3);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::MOD, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}


TEST(ASTTest, CreateBinaryOpAND)
{
    auto lhs = MAKE_CONST(1);
    auto rhs = MAKE_CONST(0);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::AND, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);

    EXPECT_EQ(result, 0);
}

TEST(ASTTest, CreateBinaryOpOR)
{
    auto lhs = MAKE_CONST(1);
    auto rhs = MAKE_CONST(0);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::OR, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpLESS)
{
    auto lhs = MAKE_CONST(3);
    auto rhs = MAKE_CONST(7);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::LS, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpLESSorEQUAL)
{
    auto lhs = MAKE_CONST(3);
    auto rhs = MAKE_CONST(7);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::LS_EQ, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpGREATER)
{
    auto lhs = MAKE_CONST(10);
    auto rhs = MAKE_CONST(5);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::GR, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpGREATERorEQUAL)
{
    auto lhs = MAKE_CONST(5);
    auto rhs = MAKE_CONST(5);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::GR_EQ, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpEQ)
{
    auto lhs = MAKE_CONST(42);
    auto rhs = MAKE_CONST(42);
    auto binOpNode = MAKE_BINARY(lhs, AST::BinaryOp::EQ, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateUnaryOpNEG)
{
    auto operand = MAKE_CONST(-42);
    auto unaryOpNode = MAKE_UNARY(operand, AST::UnaryOp::NEG);

    ASSERT_NE(unaryOpNode, nullptr);

    AST::detail::Context ctx;
    int result = unaryOpNode->eval_value(ctx);
    EXPECT_EQ(result, 42);
}

TEST(ASTTest, CreateUnaryOpNOT)
{
    auto operand = MAKE_CONST(0);
    auto unaryOpNode = MAKE_UNARY(operand, AST::UnaryOp::NOT);

    ASSERT_NE(unaryOpNode, nullptr);

    AST::detail::Context ctx;
    int result = unaryOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateAssignmentNode)
{
    auto var = MAKE_VAR("x");
    auto expr = MAKE_CONST(42);
    auto assignmentNode = MAKE_ASSIGN(var, expr);

    ASSERT_NE(assignmentNode, nullptr);

    AST::detail::Context ctx;
    ctx.varTables_.emplace_back();
    ctx.curScope_ = 0;

    int result = assignmentNode->eval_value(ctx);

    EXPECT_EQ(result, 42);
}

TEST(ASTTest, WhileNode_ConditionTrue)
{
    auto condition = MAKE_BINARY(MAKE_VAR("x"), AST::BinaryOp::LS, MAKE_CONST(10));
    auto action = MAKE_ASSIGN(MAKE_VAR("x"), MAKE_BINARY(MAKE_VAR("x"), AST::BinaryOp::ADD, MAKE_CONST(1)));
    auto whileNode = MAKE_WHILE(condition, action);

    ASSERT_NE(whileNode, nullptr);

    AST::detail::Context ctx;
    ctx.varTables_.emplace_back();
    ctx.varTables_[0]["x"] = 0;
    ctx.curScope_ = 0;

    whileNode->eval(ctx);

    EXPECT_EQ(ctx.varTables_[0]["x"], 10);
}

TEST(ASTTest, IfNode_TrueCondition)
{
    auto condition = MAKE_BINARY(MAKE_VAR("x"), AST::BinaryOp::EQ, MAKE_CONST(10));
    auto action = MAKE_ASSIGN(MAKE_VAR("y"), MAKE_CONST(20));

    auto ifNode = MAKE_IF(condition, action);

    ASSERT_NE(ifNode, nullptr);

    AST::detail::Context ctx;
    ctx.varTables_.emplace_back();
    ctx.varTables_[0]["x"] = 10;
    ctx.curScope_ = 0;

    ifNode->eval(ctx);

    EXPECT_EQ(ctx.varTables_[0]["y"], 20);
}


TEST(ASTTest, IfNode_FalseCondition)
{
    auto condition = MAKE_BINARY(MAKE_VAR("x"), AST::BinaryOp::EQ, MAKE_CONST(10));
    auto action = MAKE_ASSIGN(MAKE_VAR("y"), MAKE_CONST(20));
    auto ifNode = MAKE_IF(condition, action);

    ASSERT_NE(ifNode, nullptr);
    AST::detail::Context ctx;
    ctx.varTables_.emplace_back();
    ctx.varTables_[0]["x"] = 5;
    ctx.curScope_ = 0;

    ifNode->eval(ctx);

    EXPECT_EQ(ctx.varTables_[0].count("y"), 0);
}

TEST(ASTTest, PrintNode) {
    auto printNode = MAKE_PRINT(MAKE_VAR("x"));

    ASSERT_NE(printNode, nullptr);
    std::stringstream ss;
    AST::detail::Context ctx(ss);

    ctx.varTables_.emplace_back();
    ctx.varTables_[0]["x"] = 42;
    ctx.curScope_ = 0;

    printNode->eval(ctx);

    EXPECT_EQ(ss.str(), "42");
}

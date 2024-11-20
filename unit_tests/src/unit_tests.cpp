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

TEST(ASTTest, CreateConstant)
{
    auto constNode = CONST(42);
    ASSERT_NE(constNode, nullptr);
    EXPECT_EQ(constNode->getVal(), 42);
}

TEST(ASTTest, CreateVariable)
{
    auto varNode = VAR("x");
    ASSERT_NE(varNode, nullptr);
    EXPECT_EQ(varNode->getName(), "x");
}

TEST(ASTTest, CreateBinaryOpADD)
{
    auto lhs = CONST(1000);
    auto rhs = CONST(7);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::ADD, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);

    EXPECT_EQ(result, 1007);
}

TEST(ASTTest, CreateBinaryOpSUB)
{
    auto lhs = CONST(1000);
    auto rhs = CONST(7);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::SUB, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 993);
}

TEST(ASTTest, CreateBinaryOpMUL)
{
    auto lhs = CONST(15);
    auto rhs = CONST(3);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::MUL, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 45);
}

TEST(ASTTest, CreateBinaryOpDIV)
{
    auto lhs = CONST(42);
    auto rhs = CONST(6);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::DIV, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 7);
}

TEST(ASTTest, CreateBinaryOpDIVByZero)
{
    auto lhs = CONST(42);
    auto rhs = CONST(0);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::DIV, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    EXPECT_THROW(binOpNode->eval(ctx), std::runtime_error);
}

TEST(ASTTest, CreateBinaryOpMOD)
{
    auto lhs = CONST(10);
    auto rhs = CONST(3);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::MOD, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 1);
}


TEST(ASTTest, CreateBinaryOpAND)
{
    auto lhs = CONST(1);
    auto rhs = CONST(0);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::AND, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);

    EXPECT_EQ(result, 0);
}

TEST(ASTTest, CreateBinaryOpOR)
{
    auto lhs = CONST(1);
    auto rhs = CONST(0);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::OR, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpLESS)
{
    auto lhs = CONST(3);
    auto rhs = CONST(7);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::LS, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpLESSorEQUAL)
{
    auto lhs = CONST(3);
    auto rhs = CONST(7);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::LS_EQ, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpGREATER)
{
    auto lhs = CONST(10);
    auto rhs = CONST(5);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::GR, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpGREATERorEQUAL)
{
    auto lhs = CONST(5);
    auto rhs = CONST(5);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::GR_EQ, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpEQ)
{
    auto lhs = CONST(42);
    auto rhs = CONST(42);
    auto binOpNode = binary_op(std::move(lhs), AST::BinaryOp::EQ, std::move(rhs));

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateUnaryOpNEG)
{
    auto operand = CONST(-42);
    auto unaryOpNode = unary_op(std::move(operand), AST::UnaryOp::NEG);

    ASSERT_NE(unaryOpNode, nullptr);

    AST::detail::Context ctx;
    int result = unaryOpNode->eval(ctx);
    EXPECT_EQ(result, 42);
}

TEST(ASTTest, CreateUnaryOpNOT)
{
    auto operand = CONST(0);
    auto unaryOpNode = unary_op(std::move(operand), AST::UnaryOp::NOT);

    ASSERT_NE(unaryOpNode, nullptr);

    AST::detail::Context ctx;
    int result = unaryOpNode->eval(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateAssignmentNode)
{
    auto var = VAR("x");
    auto expr = CONST(42);
    auto assignmentNode = assignment(std::move(var), std::move(expr));

    ASSERT_NE(assignmentNode, nullptr);

    AST::detail::Context ctx;
    ctx.varTables_.emplace_back();
    ctx.curScope_ = 0;

    int result = assignmentNode->eval(ctx);

    EXPECT_EQ(result, 42);
}

TEST(ASTTest, WhileNode_ConditionTrue)
{
    auto condition = AST::binary_op(AST::variable("x"), AST::BinaryOp::LS, AST::constant(10));
    auto action = AST::assignment(AST::variable("x"), AST::binary_op(AST::variable("x"), AST::BinaryOp::ADD, AST::constant(1)));
    auto whileNode = AST::while_stmt(std::move(condition), std::move(action));

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
    auto condition = AST::binary_op(AST::variable("x"), AST::BinaryOp::EQ, AST::constant(10));
    auto action = AST::assignment(AST::variable("y"), AST::constant(20));

    auto ifNode = AST::if_stmt(std::move(condition), std::move(action));

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
    auto condition = AST::binary_op(AST::variable("x"), AST::BinaryOp::EQ, AST::constant(10));
    auto action = AST::assignment(AST::variable("y"), AST::constant(20));
    auto ifNode = AST::if_stmt(std::move(condition), std::move(action));

    ASSERT_NE(ifNode, nullptr);
    AST::detail::Context ctx;
    ctx.varTables_.emplace_back();
    ctx.varTables_[0]["x"] = 5;
    ctx.curScope_ = 0;

    ifNode->eval(ctx);

    EXPECT_EQ(ctx.varTables_[0].count("y"), 0);
}

TEST(ASTTest, PrintNode) {
    auto printNode = AST::print(AST::variable("x"));

    ASSERT_NE(printNode, nullptr);
    std::stringstream ss;
    AST::detail::Context ctx(ss);

    ctx.varTables_.emplace_back();
    ctx.varTables_[0]["x"] = 42;
    ctx.curScope_ = 0;

    printNode->eval(ctx);

    EXPECT_EQ(ss.str(), "42");
}

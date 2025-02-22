#include "ast.hh"
#include "node.hh"
#include <gtest/gtest.h> // for Test, TestInfo (ptr only), TEST
#include <string>        // for basic_string
#include <vector>

#include "test_utils.h" // for run_test

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

TEST(ASTTest, CreateConstant)
{
	AST::AST ast;
    auto constNode = ast.construct<AST::ConstantNode>(42);
    ASSERT_NE(constNode, nullptr);
    EXPECT_EQ(constNode->getVal(), 42);
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
    auto lhs = ast.construct<AST::ConstantNode>(1000);
    auto rhs = ast.construct<AST::ConstantNode>(7);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::ADD, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);

    EXPECT_EQ(result, 1007);
}

TEST(ASTTest, CreateBinaryOpSUB)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(1000);
    auto rhs = ast.construct<AST::ConstantNode>(7);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::SUB, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 993);
}

TEST(ASTTest, CreateBinaryOpMUL)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(15);
    auto rhs = ast.construct<AST::ConstantNode>(3);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::MUL, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 45);
}

TEST(ASTTest, CreateBinaryOpDIV)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(42);
    auto rhs = ast.construct<AST::ConstantNode>(6);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::DIV, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 7);
}

TEST(ASTTest, CreateBinaryOpDIVByZero)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(42);
    auto rhs = ast.construct<AST::ConstantNode>(0);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::DIV, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    EXPECT_THROW(binOpNode->eval_value(ctx), std::runtime_error);
}

TEST(ASTTest, CreateBinaryOpMOD)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(10);
    auto rhs = ast.construct<AST::ConstantNode>(3);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::MOD, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpAND)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(1);
    auto rhs = ast.construct<AST::ConstantNode>(0);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::AND, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);

    EXPECT_EQ(result, 0);
}

TEST(ASTTest, CreateBinaryOpOR)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(1);
    auto rhs = ast.construct<AST::ConstantNode>(0);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::OR, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpLESS)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(3);
    auto rhs = ast.construct<AST::ConstantNode>(7);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::LS, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpLESSorEQUAL)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(3);
    auto rhs = ast.construct<AST::ConstantNode>(7);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::LS_EQ, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpGREATER)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(10);
    auto rhs = ast.construct<AST::ConstantNode>(5);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::GR, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpGREATERorEQUAL)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(5);
    auto rhs = ast.construct<AST::ConstantNode>(5);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::GR_EQ, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateBinaryOpEQ)
{
	AST::AST ast;
    auto lhs = ast.construct<AST::ConstantNode>(42);
    auto rhs = ast.construct<AST::ConstantNode>(42);
    auto binOpNode = ast.construct<AST::BinaryOpNode>(lhs, AST::BinaryOp::EQ, rhs);

    ASSERT_NE(binOpNode, nullptr);

    AST::detail::Context ctx;
    int result = binOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateUnaryOpNEG)
{
	AST::AST ast;
    auto operand = ast.construct<AST::ConstantNode>(-42);
    auto unaryOpNode = ast.construct<AST::UnaryOpNode>(operand, AST::UnaryOp::NEG);

    ASSERT_NE(unaryOpNode, nullptr);

    AST::detail::Context ctx;
    int result = unaryOpNode->eval_value(ctx);
    EXPECT_EQ(result, 42);
}

TEST(ASTTest, CreateUnaryOpNOT)
{
	AST::AST ast;
    auto operand = ast.construct<AST::ConstantNode>(0);
    auto unaryOpNode = ast.construct<AST::UnaryOpNode>(operand, AST::UnaryOp::NOT);

    ASSERT_NE(unaryOpNode, nullptr);

    AST::detail::Context ctx;
    int result = unaryOpNode->eval_value(ctx);
    EXPECT_EQ(result, 1);
}

TEST(ASTTest, CreateAssignmentNode)
{
	AST::AST ast;
    auto var = ast.construct<AST::VariableNode>("x");
    auto expr = ast.construct<AST::ConstantNode>(42);
    auto assignmentNode = ast.construct<AST::AssignNode>(var, expr);

    ASSERT_NE(assignmentNode, nullptr);

    AST::detail::Context ctx;
    ctx.varTables_.emplace_back();
    ctx.curScope_ = 0;

    int result = assignmentNode->eval_value(ctx);

    EXPECT_EQ(result, 42);
}

TEST(ASTTest, WhileNode_ConditionTrue)
{
	AST::AST ast;
    auto condition =
        ast.construct<AST::BinaryOpNode>(ast.construct<AST::VariableNode>("x"), AST::BinaryOp::LS, ast.construct<AST::ConstantNode>(10));
    auto action = ast.construct<AST::AssignNode>(
        ast.construct<AST::VariableNode>("x"),
        ast.construct<AST::BinaryOpNode>(ast.construct<AST::VariableNode>("x"), AST::BinaryOp::ADD, ast.construct<AST::ConstantNode>(1)));
    auto whileNode = ast.construct<AST::WhileNode>(condition, action);

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
	AST::AST ast;
    auto condition =
        ast.construct<AST::BinaryOpNode>(ast.construct<AST::VariableNode>("x"), AST::BinaryOp::EQ, ast.construct<AST::ConstantNode>(10));
    auto action = ast.construct<AST::AssignNode>(ast.construct<AST::VariableNode>("y"), ast.construct<AST::ConstantNode>(20));

    auto ifNode = ast.construct<AST::IfNode>(condition, action);

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
	AST::AST ast;
    auto condition =
        ast.construct<AST::BinaryOpNode>(ast.construct<AST::VariableNode>("x"), AST::BinaryOp::EQ, ast.construct<AST::ConstantNode>(10));
    auto action = ast.construct<AST::AssignNode>(ast.construct<AST::VariableNode>("y"), ast.construct<AST::ConstantNode>(20));
    auto ifNode = ast.construct<AST::IfNode>(condition, action);

    ASSERT_NE(ifNode, nullptr);
    AST::detail::Context ctx;
    ctx.varTables_.emplace_back();
    ctx.varTables_[0]["x"] = 5;
    ctx.curScope_ = 0;

    ifNode->eval(ctx);

    EXPECT_EQ(ctx.varTables_[0].count("y"), 0);
}

TEST(ASTTest, PrintNode)
{
	AST::AST ast;
    auto printNode = ast.construct<AST::PrintNode>(ast.construct<AST::VariableNode>("x"));

    ASSERT_NE(printNode, nullptr);
    std::stringstream ss;
    AST::detail::Context ctx(ss);

    ctx.varTables_.emplace_back();
    ctx.varTables_[0]["x"] = 42;
    ctx.curScope_ = 0;

    printNode->eval(ctx);

    EXPECT_EQ(ss.str(), "42\n");
}

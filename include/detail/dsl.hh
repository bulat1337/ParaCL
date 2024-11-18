#ifndef DSL_HH
#define DSL_HH

#include "inode.hh"
#include "node.hh"
#include <memory>
#include <string>
#include <vector>

namespace AST
{
namespace detail
{

namespace detail {

inline ExprPtr constant(int value) 
{
    return std::make_unique<ConstantNode>(value);
}

inline ExprPtr variable(const std::string &name) 
{
    return std::make_unique<VariableNode>(name);
}

inline ExprPtr binary_op(ExprPtr lhs, AST::BinaryOp op, ExprPtr rhs) 
{
    return std::make_unique<BinaryOpNode>(std::move(lhs), op, std::move(rhs));
}

inline ExprPtr unary_op(ExprPtr operand, AST::UnaryOp op)
{
    return std::make_unique<UnaryOpNode>(std::move(operand), op);
}

inline StmtPtr assignment(std::unique_ptr<VariableNode> dest, ExprPtr expr ) 
{
    return std::make_unique<AssignmentNode>(name, std::move(expr));
}

inline CondStmtPtr if_stmt(ExprPtr condition, StmtPtr action) 
{
    return std::make_unique<IfNode>(std::move(condition), std::move(action));
}

inline NodePtr while_stmt(ExprPtr condition, StmtPtr scope) 
{
    return std::make_unique<WhileNode>(std::move(condition), std::move(scope));
}

inline StmtPtr print(ExprPtr expr) 
{
    return std::make_unique<PrintNode>(std::move(expr));
}

inline ExprPtr in()
{
    return std::make_unique<InNode>();
}

inline NodePtr scope(std::vector<NodePtr> statements) 
{
    return std::make_unique<BlockNode>(std::move(statements));
}


} // namespace detail 

} // namespace AST

#define LIT(val) AST::detail::literal(val)
#define VAR(name) AST::detail::variable(name)
#define BIN(op, lhs, rhs) AST::detail::binary_op(op, std::move(lhs), std::move(rhs))
#define ASSIGN(var, expr) AST::detail::assignment(var, std::move(expr))
#define PRINT(expr) AST::detail::print(std::move(expr))
#define BLOCK(stmts) AST::detail::block(std::move(stmts))
#define IF(cond, t_block, f_block) AST::detail::if_stmt(std::move(cond), std::move(t_block), std::move(f_block))
#define WHILE(cond, body) AST::detail::while_stmt(std::move(cond), std::move(body))

#endif // !DSL_HH

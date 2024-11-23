#ifndef DSL_HH
#define DSL_HH

#include "node.hh"
#include <memory>
#include <string>
#include <vector>

namespace AST
{

namespace detail
{

inline std::unique_ptr<ConstantNode> constant(int value)
{
    return std::make_unique<ConstantNode>(value);
}

inline std::unique_ptr<VariableNode> variable(const std::string &name)
{
    return std::make_unique<VariableNode>(name);
}

inline std::unique_ptr<BinaryOpNode> binary_op(ExprPtr lhs, BinaryOp op, ExprPtr rhs)
{
    return std::make_unique<BinaryOpNode>(std::move(lhs), op, std::move(rhs));
}

inline std::unique_ptr<UnaryOpNode> unary_op(ExprPtr operand, UnaryOp op)
{
    return std::make_unique<UnaryOpNode>(std::move(operand), op);
}

inline std::unique_ptr<AssignNode> assignment(std::unique_ptr<VariableNode> dest, ExprPtr expr)
{
    return std::make_unique<AssignNode>(std::move(dest), std::move(expr));
}

inline std::unique_ptr<IfNode> if_stmt(ExprPtr condition, StmtPtr action)
{
    return std::make_unique<IfNode>(std::move(condition), std::move(action));
}

inline std::unique_ptr<IfNode> if_stmt(	ExprPtr condition,
										StmtPtr action,
										ElseLikePtr else_like)
{
    return std::make_unique<IfNode>(std::move(condition),
									std::move(action),
									std::move(else_like));
}

inline std::unique_ptr<ElseIfNode> else_if_stmt(ExprPtr condition, StmtPtr action)
{
    return std::make_unique<ElseIfNode>(std::move(condition), std::move(action));
}

inline std::unique_ptr<ElseIfNode> else_if_stmt(	ExprPtr condition,
													StmtPtr action,
													ElseLikePtr else_like)
{
    return std::make_unique<ElseIfNode>(std::move(condition),
										std::move(action),
										std::move(else_like));
}

inline std::unique_ptr<ElseNode> else_stmt(StmtPtr action)
{
    return std::make_unique<ElseNode>(std::move(action));
}

inline std::unique_ptr<WhileNode> while_stmt(ExprPtr condition, StmtPtr scope)
{
    return std::make_unique<WhileNode>(std::move(condition), std::move(scope));
}

inline std::unique_ptr<PrintNode> print(ExprPtr expr)
{
    return std::make_unique<PrintNode>(std::move(expr));
}

inline std::unique_ptr<InNode> in()
{
    return std::make_unique<InNode>();
}

inline std::unique_ptr<ScopeNode> scope(std::vector<StmtPtr> statements)
{
    return std::make_unique<ScopeNode>(std::move(statements));
}

}; // namespace detail

} // namespace AST

#define MAKE_CONST(val) AST::detail::constant(val)
#define MAKE_VAR(name) AST::detail::variable(name)
#define MAKE_BINARY(lhs, op, rhs) AST::detail::binary_op(std::move(lhs), op, std::move(rhs))
#define MAKE_UNARY(operand, op) AST::detail::unary_op(std::move(operand), op)
#define MAKE_ASSIGN(var, expr) AST::detail::assignment(std::move(var), std::move(expr))
#define MAKE_PRINT(expr) AST::detail::print(std::move(expr))
#define MAKE_WHILE(cond, scope) AST::detail::while_stmt(std::move(cond), std::move(scope))

#define MAKE_IF(cond, action) AST::detail::if_stmt(std::move(cond), std::move(action))

#define MAKE_IFELSE(cond, action, else_like) \
	AST::detail::if_stmt(std::move(cond), std::move(action), std::move(else_like))

#define MAKE_ELSEIF(cond, action) AST::detail::else_if_stmt(std::move(cond), std::move(action))

#define MAKE_ELSE_IFELSE(cond, action, else_like) \
	AST::detail::else_if_stmt(std::move(cond), std::move(action), std::move(else_like))

#define MAKE_ELSE(action) AST::detail::else_stmt(std::move(action))

#define MAKE_IN() AST::detail::in()
#define MAKE_SCOPE(stmts) AST::detail::scope(std::move(stmts))

#endif // DSL_HH


#pragma once

#include <variant>

#include "context.hh"
#include "log.hh"
#include "node.hh"
#include "visitor.hh"
#include "types.hh"

namespace AST
{

namespace detail
{

class Interpreter final : public Visitor
{
  private:
    detail::Context ctx_;
    IType* buf_{};
	std::unique_ptr<IType> storage_;

  private:
	class AssignVisitor
	{
	  private:
	  	Interpreter& interpreter_;
		const AssignNode& node_;

	  private:
	  	class SrcVisitor
		{
		  private:
		  	Interpreter& interpreter_;
			std::string_view destName_;

		  public:
			SrcVisitor(Interpreter& interpreter, std::string_view destName)
			: interpreter_(interpreter)
			, destName_(destName) {}

		  	void operator()([[maybe_unused]]ExprPtr src)
			{
				MSG("It's Var-Expr assignment\n");
				interpreter_.ctx_.getVar<Integer>(destName_) =
					interpreter_.buf_->clone();
			}

			void operator()([[maybe_unused]]RepeatPtr src)
			{
				MSG("It's Var-Repeat assignment\n");
				interpreter_.ctx_.getVar<Array>(destName_) =
					interpreter_.buf_->clone();
			}

            void operator()([[maybe_unused]]ArrayInitPtr src)
            {
                MSG("It's Var-ArrayInit assignment\n");
                interpreter_.ctx_.getVar<Array>(destName_) =
                    interpreter_.buf_->clone();
            }
		};

	  public:
		explicit AssignVisitor(Interpreter& interpreter, const AssignNode& node)
		: interpreter_(interpreter)
		, node_(node) {}

	  	void operator()([[maybe_unused]]const VariablePtr dest)
		{
			std::string_view destName = node_.getDestName();

			node_.acceptSrc(interpreter_);

			std::visit(SrcVisitor(interpreter_, destName), node_.getSrc());
		}

		void operator()(const ArrayElemPtr dest)
		{
			std::string_view destName = node_.getDestName();

			dest->acceptIndex(interpreter_);
			const auto index = static_cast<Integer*>(interpreter_.buf_)->value;

			node_.acceptSrc(interpreter_);

			auto arrayPtr =
				dynamic_cast<Array*>(interpreter_.ctx_.getArray(destName).get());

			if (!arrayPtr) throw std::runtime_error("Indexing non array type\n");

			arrayPtr->assignElem(index, interpreter_.buf_);
		}

	};

  public:
    Interpreter(std::ostream &out = std::cout)
        : ctx_(out)
    {}

    int getBuf() const { return static_cast<Integer*>(buf_)->value; }

    void visit(const ConstantNode &node) override
	{
		storage_.reset();
		storage_ = std::make_unique<Integer>(node.getVal());
		buf_ = storage_.get();
	}

    void visit(const VariableNode &node) override
    {
        std::string_view name = node.getName();
        LOG("Evaluating variable: {}\n", name);

        buf_ = ctx_.getVarValue(name);
    }

    void visit(const BinaryOpNode &node) override
    {
        MSG("Evaluating Binary Operation\n");

        node.accept_left(*this);
        int leftVal = static_cast<Integer*>(buf_)->value;

        node.accept_right(*this);
        int rightVal = static_cast<Integer*>(buf_)->value;

        int result{};

        switch (node.getOp())
        {
            case BinaryOp::ADD:
                result = leftVal + rightVal;
                break;

            case BinaryOp::SUB:
                result = leftVal - rightVal;
                break;

            case BinaryOp::MUL:
                result = leftVal * rightVal;
                break;

            case BinaryOp::DIV:
                if (rightVal == 0)
					throw std::runtime_error("Divide by zero");

                result = leftVal / rightVal;
                break;

            case BinaryOp::MOD:
                result = leftVal % rightVal;
                break;

            case BinaryOp::LS:
                result = leftVal < rightVal;
                break;

            case BinaryOp::GR:
                result = leftVal > rightVal;
                break;

            case BinaryOp::LS_EQ:
                result = leftVal <= rightVal;
                break;

            case BinaryOp::GR_EQ:
                result = leftVal >= rightVal;
                break;

            case BinaryOp::EQ:
                result = leftVal == rightVal;
                break;

            case BinaryOp::NOT_EQ:
                result = leftVal != rightVal;
                break;

            case BinaryOp::AND:
                result = leftVal && rightVal;
                break;

            case BinaryOp::OR:
                result = leftVal || rightVal;
                break;

            default:
                throw std::runtime_error("Unknown binary operation");
        }

        LOG("It's {}\n", result);

		storage_.reset();
		storage_ = std::make_unique<Integer>(result);

        buf_ = storage_.get();
    }

    void visit(const ScopeNode &node) override
    {
        if (node.empty())
            return;

        MSG("Evaluating scope\n");

        ctx_.varTables_.push_back(detail::Context::VarTable());

        MSG("Scopes children:\n");
        for ([[maybe_unused]] const auto &child : node.getChildren())
        {
            LOG("{}\n", static_cast<const void *>(child));
        }

        for (const auto &child : node.getChildren())
        {
            LOG("Evaluating {}\n", static_cast<const void *>(child));
            child->accept(*this);
        }

        ctx_.varTables_.pop_back();
    }

    void visit(const UnaryOpNode &node) override
    {
        MSG("Evaluating Unary Operation\n");

        node.acceptOperand(*this);
        int operandVal = static_cast<Integer*>(buf_)->value;

		int result{};

        switch (node.getOp())
        {
            case UnaryOp::NEG:
                result = -operandVal;
                break;

            case UnaryOp::NOT:
                result = !operandVal;
                break;

            default:
                throw std::runtime_error("Unknown unary operation");
        }

		storage_.reset();
		storage_ = std::make_unique<Integer>(result);
		buf_ = storage_.get();
    }

	void visit(const AssignNode &node) override
	{
		MSG("Evaluating assignment\n");

		std::visit(AssignVisitor(*this, node), node.getDest());
	}

	void visit(const ArrayElemNode& node) override
	{
		MSG("Evaluating ArrayElemNode\n");

        node.acceptIndex(*this);
        int index = static_cast<Integer*>(buf_)->value;
		LOG("Index: {}\n", index);

		if (node.holdsVariable())
		{
			std::string_view destName = node.getName();

			LOG("Array Name: {}\n", destName);

			auto arrayPtr = dynamic_cast<Array*>(ctx_.getArray(destName).get());

			if (!arrayPtr)
			{
				std::cerr << destName << " is not an array type\n";
				throw std::runtime_error("Can't use [] to non array variables\n");
			}

			buf_ = arrayPtr->getElem(index);
		}
		else if (node.holdsArrayElem())
		{
			node.acceptName(*this);

			auto arrayPtr = dynamic_cast<Array*>(buf_);

			if (!arrayPtr)
			{
				throw std::runtime_error(
					"ArrayElem name acceptance "
					"did not result in Array\n"
				);
			}

			buf_ = arrayPtr->getElem(index);
		}
		else
		{
			throw std::runtime_error("Array element holds invalid type\n");
		}
	}

    void visit(const WhileNode &node) override
    {
        // node.acceptCond(*this);
        // int cond = buf_;

        while (node.acceptCond(*this), static_cast<Integer*>(buf_)->value)
        {
            node.acceptScope(*this);
        }
    }

    void visit(const IfElseNode &node) override
    {
        if (!node.hasCond())
        {
            // if there is no condition do it
            node.acceptAction(*this);
            return;
        }

        node.acceptCond(*this);
        int cond = static_cast<Integer*>(buf_)->value;

        if (cond)
        {
            node.acceptAction(*this);
        }
        else
        {
            if (node.hasAltAction())
                node.acceptAltAction(*this);
        }
    }

    void visit(const PrintNode &node) override
    {
        MSG("Evaluation print\n");

        node.acceptExpr(*this);
        int value = static_cast<Integer*>(buf_)->value;

        ctx_.out << value << '\n';
    }

    void visit([[maybe_unused]] const InNode &node) override
    {
        int value = 0;

        std::cin >> value;

        if (!std::cin.good())
        {
            throw std::runtime_error("Incorrect input");
        }

		storage_.reset();
		storage_ = std::make_unique<Integer>(value);
		buf_ = storage_.get();
    }

	void visit(const RepeatNode &node) override
    {
		MSG("Evaluating Repeat Node\n");

		node.acceptSize(*this);

		const auto size = static_cast<Integer*>(buf_)->value;

		LOG("Size: {}\n", size);

		if (node.hasElem())
		{
			MSG("Repeat has element prototype\n");

			node.acceptElem(*this);

			std::unique_ptr<IType> elem = buf_->clone();

			storage_.reset();
			storage_ = std::make_unique<Array>(elem.get(), size);
			buf_ = storage_.get();
		}
		else
		{
			MSG("Undefined repeat\n");

			storage_.reset();
			storage_ = std::make_unique<Array>(size);
			buf_ = storage_.get();
		}
	}

    void visit(const ArrayInitNode& node) override
    {
        MSG("Evaluating Array Init Node\n");

        size_t array_size = node.arraySize();

        std::vector<std::unique_ptr<IType>> data;

        for (int id = array_size - 1 ; id >= 0 ; --id)
        {
            node.acceptElem(id, *this);

            data.push_back(std::move(buf_->clone()));
        }

        storage_.reset();
        storage_ = std::make_unique<Array>(std::move(data));
        buf_ = storage_.get();
    }

    bool varInitialized(std::string_view varName) const
    {
        for (const auto &varTable : ctx_.varTables_)
        {
            if (varTable.count(varName) > 0)
                return true;
        }

        return false;
    }
};

}; // namespace detail

}; // namespace AST

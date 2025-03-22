#pragma once

#include <memory>

namespace AST
{

namespace detail
{

class IType
{
  public:
	virtual std::unique_ptr<IType> clone() const = 0;

	virtual ~IType() = default;
};

class Integer final : public IType
{
  public:
	int value;

  public:
	Integer(int _value = 0): value(_value) {}

	std::unique_ptr<IType> clone() const override
	{
		return std::make_unique<Integer>(value);
	}
};

class Array final : public IType
{
  private:
	std::vector<std::unique_ptr<IType>> data_;

	public:
	Array() = default;

	Array(int size)
	{
		LOG("Constructing undefind array of size {}\n", size);

		data_.resize(size);
	}

	Array(const IType* elem, int size)
	{
		data_.reserve(size);

		for (size_t id = 0; id < size; ++id)
		{
			data_.push_back(elem->clone());
		}
	}

	std::unique_ptr<IType> clone() const override
	{
		auto clonedArray = std::make_unique<Array>();

		clonedArray->data_.reserve(data_.size());

		for (const auto& elem : data_)
		{
			clonedArray->data_.push_back(elem ? elem->clone() : nullptr);
		}

		return clonedArray;
	}

	IType* getElem(int index)
	{
		return data_[index].get();
	}

	void assignElem(int index, IType* elem)
	{
		data_[index].reset();

		data_[index] = elem->clone();
	}
};

}; // detail

}; // AST

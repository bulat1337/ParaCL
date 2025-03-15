#pragma once


namespace AST
{

namespace detail
{

class IType
{
  public:
	virtual ~IType() = default;
};

class Integer final : public IType
{
  public:
	int value;

  public:
	Integer(int _value = 0): value(_value) {}
};

class Array final : public IType
{
  public:
	IType* data{};
	size_t size{};
};

}; // detail

}; // AST

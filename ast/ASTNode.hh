#pragma once

#include "Statement.hh"
#include <string>

class PrintVisitor;

class INode
{
  public:
    virtual ~INode() = default;
    virtual void accept(PrintVisitor& visitor) = 0;
    virtual std::string toString() const = 0;
};


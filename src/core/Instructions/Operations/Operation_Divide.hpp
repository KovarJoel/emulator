#pragma once

#include "Bases/Operation_Base.hpp"

namespace core::instructions::operations {
  class Divide : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };

  class DivideUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };
}

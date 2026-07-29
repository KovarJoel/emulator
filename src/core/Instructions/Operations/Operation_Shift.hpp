#pragma once

#include "Bases/Operation_Base.hpp"

namespace emulator::core::instructions::operations {
  class ShiftLeftLogical : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };

  class ShiftRightLogical : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };

  class ShiftRightArithmetic : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };
}

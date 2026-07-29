#pragma once

#include "Bases/Operation_Base.hpp"

namespace emulator::core::instructions::operations {
  class Remainder : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };

  class RemainderUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };
}

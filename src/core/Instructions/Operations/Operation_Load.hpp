#pragma once

#include "Bases/Operation_Base_Memory.hpp"

namespace emulator::core::instructions::operations {
  class Load : public bases::BaseMemory {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };

  class LoadZeroExtend : public bases::BaseMemory {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };
}

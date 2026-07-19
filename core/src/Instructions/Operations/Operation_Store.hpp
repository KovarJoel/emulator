#pragma once

#include "Bases/Operation_Base_Memory.hpp"

namespace emulator::core::instructions::operations {
  class Store : public bases::BaseMemory {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };
}

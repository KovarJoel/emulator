#pragma once

#include "Bases/Operation_Base.hpp"

namespace emulator::core::instructions::operations {
  class Cycle : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      state.registers[data.getDestRegisterAddr()].set(state.cycle_count);

      updateFlagsZeroAndSign(data, state);
      advancePC(data, state);
    }
  };
}

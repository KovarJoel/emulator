#pragma once

#include "Operation_Base.hpp"

namespace emulator::core::instructions::operations::bases {
  class BaseMemory : public Base {
  protected:
    uint32_t calculateAddress(const InstructionData& data, const ProcessorState& state) const;
  };
}

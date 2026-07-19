#pragma once

#include "Bases/Operation_Base.hpp"

namespace emulator::core::instructions::operations {
  class Halt : public bases::Base {
  public:
    void execute(const InstructionData&, ProcessorState&) const {
      throw exceptions::Halt{};
    }
  };
}

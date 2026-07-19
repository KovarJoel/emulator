#pragma once

#include "Bases/Operation_Base.hpp"

namespace emulator::core::instructions::operations {
  class And : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };

  class Or : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };
  
  class Xor : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;
  };
}

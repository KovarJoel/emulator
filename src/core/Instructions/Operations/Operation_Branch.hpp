#pragma once

#include "Bases/Operation_Base.hpp"

namespace core::instructions::operations {
  namespace {
    uint32_t getTargetAddress(const InstructionData& data, const ProcessorState& state) {
      const auto& src = data.getSources()[0];

      if (src.getSourceMode() == RegisterOrImmediate::SourceMode::Register) {
        return src.getValue(state.registers);
      }

      constexpr uint32_t ZERO_EXTEND_MASK = 0x00FF'FFFF;
      return src.getImmediateValue() & ZERO_EXTEND_MASK;
    }
  }

  class Jump : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      state.registers.getPC().set(getTargetAddress(data, state));
    }
  };

  class BranchEqual : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      if (state.registers.getFLAGS().getBit(Register::FlagIndex::Zero)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchNotEqual : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      if (!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchGreater : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (!flags.getBit(Zero) && flags.getBit(Sign) == flags.getBit(Overflow)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchLess : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (!flags.getBit(Zero) && flags.getBit(Sign) != flags.getBit(Overflow)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchGreaterEqual : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (flags.getBit(Zero) || flags.getBit(Sign) == flags.getBit(Overflow)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchLessEqual : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (flags.getBit(Zero) || flags.getBit(Sign) != flags.getBit(Overflow)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchGreaterUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (!flags.getBit(Zero) && !flags.getBit(Carry)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchLessUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (flags.getBit(Carry)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchGreaterEqualUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (!flags.getBit(Carry)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchLessEqualUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (flags.getBit(Zero) || flags.getBit(Sign) != flags.getBit(Overflow)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchCarry : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      if (state.registers.getFLAGS().getBit(Register::FlagIndex::Carry)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchNotCarry : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      if (!state.registers.getFLAGS().getBit(Register::FlagIndex::Carry)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchOverflow : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      if (state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };

  class BranchNotOverflow : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      if (!state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow)) {
        state.registers.getPC().set(getTargetAddress(data, state));
      } else {
        advancePC(data, state);
      }
    }
  };
}

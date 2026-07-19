#pragma once

#include "Bases/Operation_Base.hpp"

namespace emulator::core::instructions::operations {
  class Jump : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const {
      state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
    }
  };

  class BranchEqual : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      if (state.registers.getFLAGS().getBit(Register::FlagIndex::Zero)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchNotEqual : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      if (!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchGreater : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (!flags.getBit(Zero) && flags.getBit(Sign) == flags.getBit(Overflow)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchLess : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (!flags.getBit(Zero) && flags.getBit(Sign) != flags.getBit(Overflow)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchGreaterEqual : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (flags.getBit(Zero) || flags.getBit(Sign) == flags.getBit(Overflow)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchLessEqual : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (flags.getBit(Zero) || flags.getBit(Sign) != flags.getBit(Overflow)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchGreaterUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (!flags.getBit(Zero) && !flags.getBit(Carry)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchLessUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (flags.getBit(Carry)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchGreaterEqualUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (!flags.getBit(Carry)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchLessEqualUnsigned : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      const auto flags = state.registers.getFLAGS();
      using enum Register::FlagIndex;

      if (flags.getBit(Zero) || flags.getBit(Sign) != flags.getBit(Overflow)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchCarry : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      if (state.registers.getFLAGS().getBit(Register::FlagIndex::Carry)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchNotCarry : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      if (!state.registers.getFLAGS().getBit(Register::FlagIndex::Carry)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchOverflow : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      if (state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };

  class BranchNotOverflow : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) {
      if (!state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow)) {
        state.registers.getPC().set(data.getSources()[0].getValue(state.registers));
      }
      else {
        advancePC(data, state);
      }
    }
  };
}

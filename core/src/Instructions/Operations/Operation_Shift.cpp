#include "Operation_Shift.hpp"

namespace emulator::core::instructions::operations {
  namespace {
    void updateFlagOverflow(ProcessorState& state, uint32_t src, uint32_t dest) {
      if ((src & (1u << Register::INDEX_MSB)) != (dest & (1u << Register::INDEX_MSB))) {
        state.registers.getFLAGS().setBit(Register::FlagIndex::Overflow);
      }
    }
  }

  void ShiftLeftLogical::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t src = data.getSources()[0].getValue(state.registers);
    const uint32_t positions = data.getSources()[1].getValue(state.registers);
    const uint32_t dest = positions >= 32 ? 0 : src << positions;

    state.registers[data.getDestRegisterAddr()].set(dest);

    if (positions != 0) {
      state.registers.getFLAGS().setBit(Register::FlagIndex::Carry,
        positions >= 32 ?
        0 :
        src & (1u << (32 - positions))
      );
    }

    updateFlagsZeroAndSign(data, state);
    updateFlagOverflow(state, src, dest);

    advancePC(data, state);
  }

  void ShiftRightLogical::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t src = data.getSources()[0].getValue(state.registers);
    const uint32_t positions = data.getSources()[1].getValue(state.registers);
    const uint32_t dest = positions >= 32 ? 0 : src >> positions;

    state.registers[data.getDestRegisterAddr()].set(dest);

    if (positions != 0) {
      state.registers.getFLAGS().setBit(Register::FlagIndex::Carry,
        positions >= 32 ?
        0 :
        src & (1u << (positions - 1))
      );
    }

    updateFlagsZeroAndSign(data, state);
    updateFlagOverflow(state, src, dest);

    advancePC(data, state);
  }

  void ShiftRightArithmetic::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t src = data.getSources()[0].getValue(state.registers);
    const uint32_t positions = data.getSources()[1].getValue(state.registers);
    const uint32_t dest =
      positions >= 32 ?
      (-1) :
      (src >> positions) | ((~(1u << (positions - 1))) << (32 - positions))
    ;

    state.registers[data.getDestRegisterAddr()].set(dest);

    if (positions != 0) {
      state.registers.getFLAGS().setBit(Register::FlagIndex::Carry,
        positions >= 32 ?
        1 :
        src & (1u << (positions - 1))
      );
    }

    updateFlagsZeroAndSign(data, state);
    updateFlagOverflow(state, src, dest);

    advancePC(data, state);
  }
}

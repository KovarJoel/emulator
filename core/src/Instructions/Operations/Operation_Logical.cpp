#include "Operation_Logical.hpp"

namespace emulator::core::instructions::operations {
  void And::execute(const InstructionData& data, ProcessorState& state) const {
    state.registers[data.getDestRegisterAddr()].set(
      data.getSources()[0].getValue(state.registers) &
      data.getSources()[1].getValue(state.registers)
    );

    updateFlagsZeroAndSign(data, state);
    advancePC(data, state);
  }

  void Or::execute(const InstructionData& data, ProcessorState& state) const {
    state.registers[data.getDestRegisterAddr()].set(
      data.getSources()[0].getValue(state.registers) |
      data.getSources()[1].getValue(state.registers)
    );

    updateFlagsZeroAndSign(data, state);
    advancePC(data, state);
  }

  void Xor::execute(const InstructionData& data, ProcessorState& state) const {
    state.registers[data.getDestRegisterAddr()].set(
      data.getSources()[0].getValue(state.registers) ^
      data.getSources()[1].getValue(state.registers)
    );

    updateFlagsZeroAndSign(data, state);
    advancePC(data, state);
  }
}

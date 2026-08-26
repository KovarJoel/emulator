#include "Operation_Logical.hpp"

namespace core::instructions::operations {
  void And::execute(const InstructionData& data, ProcessorState& state) const {
    const auto result = data.getSources()[0].getValue(state.registers)
                        & data.getSources()[1].getValue(state.registers);

    state.registers[data.getDestRegisterAddr()].set(result);

    updateFlagsZeroAndSign(result, state);
    advancePC(data, state);
  }

  void Or::execute(const InstructionData& data, ProcessorState& state) const {
    const auto result = data.getSources()[0].getValue(state.registers)
                        | data.getSources()[1].getValue(state.registers);

    state.registers[data.getDestRegisterAddr()].set(result);

    updateFlagsZeroAndSign(result, state);
    advancePC(data, state);
  }

  void Xor::execute(const InstructionData& data, ProcessorState& state) const {
    const auto result = data.getSources()[0].getValue(state.registers)
                        ^ data.getSources()[1].getValue(state.registers);

    state.registers[data.getDestRegisterAddr()].set(result);

    updateFlagsZeroAndSign(result, state);
    advancePC(data, state);
  }
}

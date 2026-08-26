#include "Operation_Rotate.hpp"

#include <bit>

namespace core::instructions::operations {
  void RotateLeft::execute(const InstructionData& data, ProcessorState& state) const {
    const auto result = std::rotl(
      data.getSources()[0].getValue(state.registers),
      data.getSources()[1].getValue(state.registers) % 32
    );

    state.registers[data.getDestRegisterAddr()].set(result);

    updateFlagsZeroAndSign(result, state);
    advancePC(data, state);
  }

  void RotateRight::execute(const InstructionData& data, ProcessorState& state) const {
    const auto result = std::rotr(
      data.getSources()[0].getValue(state.registers),
      data.getSources()[1].getValue(state.registers) % 32
    );

    state.registers[data.getDestRegisterAddr()].set(result);

    updateFlagsZeroAndSign(result, state);
    advancePC(data, state);
  }
}

#include "Operation_Rotate.hpp"

#include <bit>

namespace emulator::core::instructions::operations {
    void RotateLeft::execute(const InstructionData& data, ProcessorState& state) const {
      state.registers[data.getDestRegisterAddr()].set(
        std::rotl(
          data.getSources()[0].getValue(state.registers),
          data.getSources()[1].getValue(state.registers) % 32
        )
      );

      updateFlagsZeroAndSign(data, state);
      advancePC(data, state);
    }

    void RotateRight::execute(const InstructionData& data, ProcessorState& state) const {
      state.registers[data.getDestRegisterAddr()].set(
        std::rotr(
          data.getSources()[0].getValue(state.registers),
          data.getSources()[1].getValue(state.registers) % 32
        )
      );

      updateFlagsZeroAndSign(data, state);
      advancePC(data, state);
    }
}

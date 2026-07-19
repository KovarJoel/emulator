#include "Operation_Add.hpp"

#include <limits>

namespace emulator::core::instructions::operations {
  void Add::execute(const InstructionData& data, ProcessorState& state) const {
    const uint64_t value1 = data.getSources()[0].getValue(state.registers);
    const uint64_t value2 = data.getSources()[0].getValue(state.registers);
    const uint64_t result = value1 + value2;

    state.registers[data.getDestRegisterAddr()].set<uint32_t>(result);

    if (result > std::numeric_limits<uint32_t>::max()) {
      state.registers.getFLAGS().setBit(Register::FlagIndex::Carry);
    }

    const bool value1_signed = getWordBit(value1, Register::INDEX_MSB);
    const bool value2_signed = getWordBit(value2, Register::INDEX_MSB);
    const bool result_signed = getWordBit(result, Register::INDEX_MSB);
    if (value1_signed == value2_signed && value1_signed != result_signed) {
      state.registers.getFLAGS().setBit(Register::FlagIndex::Overflow);
    }

    updateFlagsZeroAndSign(data, state);
    advancePC(data, state);
  }
}

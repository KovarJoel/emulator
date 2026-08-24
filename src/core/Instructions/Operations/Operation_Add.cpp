#include "Operation_Add.hpp"

#include <limits>

namespace core::instructions::operations {
  void Add::execute(const InstructionData& data, ProcessorState& state) const {
    const uint64_t value1 = data.getSources()[0].getValue(state.registers);
    const uint64_t value2 = data.getSources()[1].getValue(state.registers);
    const uint64_t result = value1 + value2;

    state.registers[data.getDestRegisterAddr()].set<uint32_t>(static_cast<uint32_t>(result));

    state.registers.getFLAGS().setBit(
      Register::FlagIndex::Carry,
      result > std::numeric_limits<uint32_t>::max()
    );

    const bool value1_signed = getWordBit(static_cast<uint32_t>(value1), Register::INDEX_MSB);
    const bool value2_signed = getWordBit(static_cast<uint32_t>(value2), Register::INDEX_MSB);
    const bool result_signed = getWordBit(static_cast<uint32_t>(result), Register::INDEX_MSB);
    state.registers.getFLAGS().setBit(
      Register::FlagIndex::Overflow,
      value1_signed == value2_signed && value1_signed != result_signed
    );

    updateFlagsZeroAndSign(static_cast<uint32_t>(result), state);
    advancePC(data, state);
  }
}

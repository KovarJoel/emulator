#include "Operation_Subtract.hpp"

namespace emulator::core::instructions::operations {
  void Subtract::execute(const InstructionData& data, ProcessorState& state) const {
    const uint64_t value1 = data.getSources()[0].getValue(state.registers);      
    const uint64_t value2 = data.getSources()[1].getValue(state.registers);      
    const uint64_t result = value1 - value2;

    state.registers[data.getDestRegisterAddr()].set<uint32_t>(result);

    state.registers.getFLAGS().setBit(
      Register::FlagIndex::Carry,
      value1 < value2
    );

    const bool value1_signed = getWordBit(value1, Register::INDEX_MSB);
    const bool value2_signed = getWordBit(value2, Register::INDEX_MSB);
    const bool result_signed = getWordBit(result, Register::INDEX_MSB);
    state.registers.getFLAGS().setBit(
      Register::FlagIndex::Overflow,
      value1_signed != value2_signed && value1_signed != result_signed
    );

    updateFlagsZeroAndSign(data, state);
    advancePC(data, state);
  }
}

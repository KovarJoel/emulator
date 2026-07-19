#include "Operation_Divide.hpp"

#include "Exceptions.hpp"

namespace emulator::core::instructions::operations {
  void Divide::execute(const InstructionData& data, ProcessorState& state) const {
    const auto value1 = std::bit_cast<int32_t>(data.getSources()[0].getValue(state.registers));
    const auto value2 = std::bit_cast<int32_t>(data.getSources()[1].getValue(state.registers));

    if (value2 == 0) {
      throw exceptions::DivideByZero{};
    }

    const auto result = value1 / value2;
    
    state.registers[data.getDestRegisterAddr()].set(result);

    updateFlagsZeroAndSign(data, state);
    advancePC(data, state);
  }

  void DivideUnsigned::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t value1 = data.getSources()[0].getValue(state.registers);      
    const uint32_t value2 = data.getSources()[1].getValue(state.registers);
    
    if (value2 == 0) {
      throw exceptions::DivideByZero{};
    }
    
    const uint32_t result = value1 / value2;

    state.registers[data.getDestRegisterAddr()].set(result);

    if (state.registers[data.getDestRegisterAddr()].get<uint32_t>() == 0) {
      state.registers.getFLAGS().setBit(Register::FlagIndex::Zero);
    }
    advancePC(data, state);
  }
}

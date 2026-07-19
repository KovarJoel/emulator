#include "Operation_Load.hpp"

#include "Exceptions.hpp"

namespace emulator::core::instructions::operations {
  void Load::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t address = calculateAddress(data, state);

    switch (data.getWidth()) {
      case InstructionData::Width::Byte:
        state.registers[data.getDestRegisterAddr()].set(
          state.memory.get<int8_t>(address)
        );
        break;
      case InstructionData::Width::HalfWord:
        state.registers[data.getDestRegisterAddr()].set(
          state.memory.get<int16_t>(address)
        );
        break;
      case InstructionData::Width::Word:
        state.registers[data.getDestRegisterAddr()].set(
          state.memory.get<int32_t>(address)
        );
        break;
      default:
        throw exceptions::InvalidOperationWidth{};
    }

    updateFlagsZeroAndSign(data, state);
    advancePC(data, state);
  }
  
  void LoadZeroExtend::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t address = calculateAddress(data, state);

    switch (data.getWidth()) {
      case InstructionData::Width::Byte:
        state.registers[data.getDestRegisterAddr()].set(
          state.memory.get<uint8_t>(address)
        );
        break;
      case InstructionData::Width::HalfWord:
        state.registers[data.getDestRegisterAddr()].set(
          state.memory.get<uint16_t>(address)
        );
        break;
      case InstructionData::Width::Word:
        state.registers[data.getDestRegisterAddr()].set(
          state.memory.get<uint32_t>(address)
        );
        break;
      default:
        throw exceptions::InvalidOperationWidth{};
    }

    updateFlagsZeroAndSign(data, state);
    advancePC(data, state);
  }
}

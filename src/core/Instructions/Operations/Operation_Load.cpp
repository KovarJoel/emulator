#include "Operation_Load.hpp"

#include "core/Exceptions.hpp"

#include <mutex>

namespace core::instructions::operations {
  void Load::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t address = calculateAddress(data, state);

    uint32_t result {};
    std::lock_guard<std::mutex> lock { state.memory.getMutex() };

    switch (data.getWidth()) {
    case InstructionData::Width::Byte:
      result = state.memory.get<int8_t>(address);
      break;
    case InstructionData::Width::HalfWord:
      result = state.memory.get<int16_t>(address);
      break;
    case InstructionData::Width::Word:
      result = state.memory.get<int32_t>(address);
      break;
    default:
      throw exceptions::InvalidOperationWidth {};
    }

    state.registers[data.getDestRegisterAddr()].set(result);

    updateFlagsZeroAndSign(result, state);
    advancePC(data, state);
  }

  void LoadZeroExtend::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t address = calculateAddress(data, state);

    uint32_t result = 0;
    std::lock_guard<std::mutex> lock { state.memory.getMutex() };

    switch (data.getWidth()) {
    case InstructionData::Width::Byte:
      result = state.memory.get<uint8_t>(address);
      break;
    case InstructionData::Width::HalfWord:
      result = state.memory.get<uint16_t>(address);
      break;
    case InstructionData::Width::Word:
      result = state.memory.get<uint32_t>(address);
      break;
    default:
      throw exceptions::InvalidOperationWidth {};
    }

    state.registers[data.getDestRegisterAddr()].set(result);

    updateFlagsZeroAndSign(result, state);
    advancePC(data, state);
  }
}

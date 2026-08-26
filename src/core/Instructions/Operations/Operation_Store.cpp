#include "Operation_Store.hpp"

#include "core/Exceptions.hpp"

#include <mutex>

namespace core::instructions::operations {
  void Store::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t address = calculateAddress(data, state);

    std::lock_guard<std::mutex> lock { state.memory.getMutex() };

    switch (data.getWidth()) {
    case InstructionData::Width::Byte:
      state.memory.set(address, state.registers[data.getDestRegisterAddr()].get<uint8_t>());
      break;
    case InstructionData::Width::HalfWord:
      state.memory.set(address, state.registers[data.getDestRegisterAddr()].get<uint16_t>());
      break;
    case InstructionData::Width::Word:
      state.memory.set(address, state.registers[data.getDestRegisterAddr()].get<uint32_t>());
      break;
    default:
      throw exceptions::InvalidOperationWidth {};
    }

    advancePC(data, state);
  }
}

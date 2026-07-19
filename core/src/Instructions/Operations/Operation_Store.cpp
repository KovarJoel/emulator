#include "Operation_Store.hpp"

#include "Exceptions.hpp"

namespace emulator::core::instructions::operations {
  void Store::execute(const InstructionData& data, ProcessorState& state) const {
    const uint32_t address = calculateAddress(data, state);

    switch (data.getWidth()) {
      case InstructionData::Width::Byte:
        state.memory.set(address,
          state.registers[data.getDestRegisterAddr()].get<uint8_t>()  
        );
        break;
      case InstructionData::Width::HalfWord:
        state.memory.set(address,
          state.registers[data.getDestRegisterAddr()].get<uint16_t>()  
        );
        break;
      case InstructionData::Width::Word:
        state.memory.set(address,
          state.registers[data.getDestRegisterAddr()].get<uint32_t>()  
        );
        break;
      default:
        throw exceptions::InvalidOperationWidth{}; 
    }

    advancePC(data, state);
  }    
}

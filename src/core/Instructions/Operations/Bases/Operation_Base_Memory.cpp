#include "Operation_Base_Memory.hpp"

#include "Exceptions.hpp"

#include <array>

namespace emulator::core::instructions::operations::bases {
  uint32_t BaseMemory::calculateAddress(const InstructionData& data, const ProcessorState& state) const {
    const auto sources = data.getSources();
    const std::array<uint32_t, 2> values = {
      sources[0].getValue(state.registers),
      sources[1].getValue(state.registers)
    };

    uint32_t address = 0;

    if (sources[0].getSourceMode() == InstructionData::SourceMode::Immediate &&
        sources[1].getSourceMode() == InstructionData::SourceMode::Immediate) {
      throw exceptions::InvalidAddressignMode{};
    }
    else if ( sources[0].getSourceMode() == InstructionData::SourceMode::Register &&
              sources[1].getSourceMode() == InstructionData::SourceMode::Immediate) {
      throw exceptions::InvalidAddressignMode{};
    }
    else if ( sources[0].getSourceMode() == InstructionData::SourceMode::Immediate &&
              sources[1].getSourceMode() == InstructionData::SourceMode::Register) {
      address = values[0] + values[1];
    }
    else {
      address = values[0] + widthToBytes(data.getWidth()) * values[1];
    }

    return address;
  }
}

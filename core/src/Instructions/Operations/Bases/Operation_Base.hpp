#pragma once

#include "ProcessorState.hpp"
#include "Instructions/InstructionData.hpp"

#include <cstdint>
#include <cstddef>

namespace emulator::core::instructions::operations::bases {
  class Base {
  public:
    void execute(const InstructionData&, ProcessorState&) {
      throw exceptions::InvalidOperation{};
    }

    constexpr static uint32_t widthToBytes(InstructionData::Width width) {
      switch (width) {
        case InstructionData::Width::Byte:     return 1;
        case InstructionData::Width::HalfWord: return 2;
        case InstructionData::Width::Word:     return 4;
      }
      return 0;
    }

    constexpr static bool isValidWidth(InstructionData::Width width) {
      return widthToBytes(width) != 0;
    }

    constexpr static bool getWordBit(uint32_t word, size_t bit) {
      return word & (1u << bit);
    }
    
    void updateFlagsZeroAndSign(const InstructionData& data, ProcessorState& state) const;
    void advancePC(const InstructionData& data, ProcessorState& state) const;
  };
}

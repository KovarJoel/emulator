#pragma once

#include "core/Instructions/InstructionData.hpp"
#include "core/ProcessorState.hpp"

#include <cstddef>
#include <cstdint>

namespace core::instructions::operations::bases {
  class Base {
  public:
    void execute(const InstructionData&, ProcessorState&) const {
      throw exceptions::InvalidOperation {};
    }

  protected:
    static constexpr uint32_t widthToBytes(InstructionData::Width width) {
      switch (width) {
      case InstructionData::Width::Byte:
        return 1;
      case InstructionData::Width::HalfWord:
        return 2;
      case InstructionData::Width::Word:
        return 4;
      }
      return 0;
    }

    static constexpr bool isValidWidth(InstructionData::Width width) {
      return widthToBytes(width) != 0;
    }

    static constexpr bool getWordBit(uint32_t word, size_t bit) {
      return word & (1u << bit);
    }

    void updateFlagsZeroAndSign(uint32_t result, ProcessorState& state) const;
    void advancePC(const InstructionData& data, ProcessorState& state) const;
  };
}

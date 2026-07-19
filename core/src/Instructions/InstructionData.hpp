#pragma once

#include "Operations.hpp"
#include "RegisterOrImmediate.hpp"
#include "ProcessorState.hpp"

#include <array>
#include <cstdint>

namespace emulator::core::instructions {
  class InstructionData {
  public:
    enum class Width {
      Byte, HalfWord, Word
    };
    using SourceMode = RegisterOrImmediate::SourceMode;

    constexpr static size_t ENCODING_SIZE_BRANCH = 4;
    constexpr static size_t ENCODING_SIZE_MIN = 3;
    constexpr static size_t ENCODING_SIZE_IMMEDIATE = 3;

    constexpr static size_t ENCODING_WIDTH_OPCODE = 8;
    constexpr static size_t ENCODING_WIDTH_WIDTH = 2;
    constexpr static size_t ENCODING_WIDTH_REGISTER_ADDRESS = 4;
    constexpr static size_t ENCODING_WIDTH_SOURCE_MODE = 1;
    constexpr static size_t ENCODING_WIDTH_IMMEDIATE = ENCODING_SIZE_IMMEDIATE * 8;

  public:
    InstructionData();
    InstructionData(Opcode opcode, Width width, uint32_t dest_register_addr, const std::array<RegisterOrImmediate, 2>& sources);

    Opcode getOpcode() const;
    Width getWidth() const;
    uint32_t getDestRegisterAddr() const;
    const std::array<RegisterOrImmediate, 2>& getSources() const;

    size_t getEncodingSize() const;
    
  private:
    bool isValid() const;
  
  private:
    Opcode m_opcode{};
    Width m_width{};
    uint32_t m_dest_register_addr{};
    std::array<RegisterOrImmediate, 2> m_sources{};
  };
}

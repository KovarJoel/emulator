#pragma once

#include "Operations.hpp"
#include "RegisterOrImmediate.hpp"

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace core::instructions {
  class InstructionData {
  public:
    enum class Width { Byte, HalfWord, Word };
    using SourceMode = RegisterOrImmediate::SourceMode;

    static constexpr uint32_t ENCODING_SIZE_BRANCH = 4;
    static constexpr uint32_t ENCODING_SIZE_MIN = 3;
    static constexpr uint32_t ENCODING_SIZE_IMMEDIATE = 3;

    static constexpr uint32_t ENCODING_WIDTH_OPCODE = 8;
    static constexpr uint32_t ENCODING_WIDTH_WIDTH = 2;
    static constexpr uint32_t ENCODING_WIDTH_REGISTER_ADDRESS = 4;
    static constexpr uint32_t ENCODING_WIDTH_SOURCE_MODE = 1;
    static constexpr uint32_t ENCODING_WIDTH_IMMEDIATE = ENCODING_SIZE_IMMEDIATE * 8;

  public:
    InstructionData();
    InstructionData(
      Opcode opcode,
      Width width,
      uint32_t dest_register_addr,
      const std::array<RegisterOrImmediate, 2>& sources
    );

    Opcode getOpcode() const;
    Width getWidth() const;
    uint32_t getDestRegisterAddr() const;
    const std::array<RegisterOrImmediate, 2>& getSources() const;

    uint32_t getEncodingSize() const;
    std::vector<std::byte> encode() const;

    void decode(std::span<const std::byte> data);

    bool operator==(const InstructionData& other) const = default;

  private:
    bool isValid() const;

    static uint32_t decodeImmediate(std::span<const std::byte> data);
    static Width decodeWidth(uint16_t remaining_data);

  private:
    Opcode m_opcode {};
    Width m_width {};
    uint32_t m_dest_register_addr {};
    std::array<RegisterOrImmediate, 2> m_sources {};
  };
}

#include "InstructionData.hpp"

#include "core/Exceptions.hpp"
#include "core/Instructions/Operations.hpp"
#include "core/Instructions/RegisterOrImmediate.hpp"
#include "core/RegisterBank.hpp"

#include <cstdint>

namespace core::instructions {
  InstructionData::InstructionData()
    : m_opcode { Opcode::ADD }, m_width { Width::Word }, m_dest_register_addr { 0 }, m_sources {} {
    // equivalent to NOP (ADD r0, r0, r0)
    if (!isValid()) {
      throw exceptions::BadInstructionValue {};
    }
  }

  InstructionData::InstructionData(
    Opcode opcode,
    Width width,
    uint32_t dest_register_addr,
    const std::array<RegisterOrImmediate, 2>& sources
  )
    : m_opcode { opcode },
      m_width { width },
      m_dest_register_addr { dest_register_addr },
      m_sources { sources } {
    if (!isValid()) {
      throw exceptions::BadInstructionValue {};
    }
  }

  Opcode InstructionData::getOpcode() const {
    return m_opcode;
  }

  InstructionData::Width InstructionData::getWidth() const {
    return m_width;
  }

  uint32_t InstructionData::getDestRegisterAddr() const {
    return m_dest_register_addr;
  }

  const std::array<RegisterOrImmediate, 2>& InstructionData::getSources() const {
    return m_sources;
  }

  uint32_t InstructionData::getEncodingSize() const {
    if (opcodeGetType(m_opcode) == OpcodeType::Branch) {
      return ENCODING_SIZE_BRANCH;
    } else {
      uint32_t immediates_count = 0;
      for (const auto& src : m_sources) {
        if (src.getSourceMode() == RegisterOrImmediate::SourceMode::Immediate) {
          ++immediates_count;
        }
      }
      return ENCODING_SIZE_MIN + immediates_count * ENCODING_SIZE_IMMEDIATE;
    }
  }

  std::vector<std::byte> InstructionData::encode() const {
    std::vector<std::byte> encoding(getEncodingSize());

    encoding[0] = static_cast<std::byte>(m_opcode);

    if (opcodeGetType(m_opcode) == OpcodeType::Branch) {
      const auto jump_target = m_sources[0].getImmediateValue();
      memcpy(&encoding[1], &jump_target, ENCODING_SIZE_IMMEDIATE);
      return encoding;
    }

    uint16_t data {};
    data = static_cast<uint16_t>(m_width);

    data <<= ENCODING_WIDTH_REGISTER_ADDRESS;
    data |= static_cast<uint16_t>(m_dest_register_addr);

    for (uint32_t immediate_count = 0; const auto& src : m_sources) {
      data <<= ENCODING_WIDTH_REGISTER_ADDRESS;
      if (src.getSourceMode() == SourceMode::Register) {
        data |= static_cast<uint16_t>(src.getRegisterAddress());
      } else {
        const auto immediate = src.getImmediateValue();
        memcpy(
          &encoding[ENCODING_SIZE_MIN + immediate_count * ENCODING_SIZE_IMMEDIATE],
          &immediate,
          ENCODING_SIZE_IMMEDIATE
        );
        ++immediate_count;
      }

      data <<= ENCODING_WIDTH_SOURCE_MODE;
      data |= static_cast<uint16_t>(src.getSourceMode());
    }

    encoding[1] = static_cast<std::byte>(data >> 8);
    encoding[2] = static_cast<std::byte>(data);

    return encoding;
  }

  uint32_t InstructionData::decodeImmediate(std::span<const std::byte> data) {
    if (data.size_bytes() < ENCODING_SIZE_IMMEDIATE) {
      throw exceptions::InvalidExecuteAccess {};
    }

    uint32_t immediate {};
    for (size_t i = 0; i < ENCODING_SIZE_IMMEDIATE; ++i) {
      immediate <<= 8;
      immediate |= static_cast<uint8_t>(data[ENCODING_SIZE_IMMEDIATE - 1 - i]);
    }
    return immediate;
  }

  void InstructionData::decode(std::span<const std::byte> data) {
    if (data.size_bytes() < ENCODING_SIZE_MIN) {
      throw exceptions::InvalidExecuteAccess {};
    }

    const Opcode opcode = static_cast<Opcode>(data[0]);
    if (!opcodeIsValid(opcode)) {
      throw exceptions::InvalidOperation {};
    }

    if (opcodeGetType(opcode) == OpcodeType::Branch) {
      const auto jump_target = decodeImmediate(data.subspan(1));
      *this = InstructionData { opcode,
                                Width::Word,
                                0,
                                { RegisterOrImmediate { SourceMode::Immediate, jump_target } } };
      return;
    }

    const uint16_t remaining_data =
      (static_cast<uint16_t>(data[1]) << 8) | static_cast<uint8_t>(data[2]);

    auto decodeField = [&remaining_data, offset = 0u]<typename T>(uint32_t encoding_width) mutable {
      offset += encoding_width;
      const auto mask = ((1 << encoding_width) - 1) << (16 - offset);
      return static_cast<T>((remaining_data & mask) >> (16 - offset));
    };

    const auto width = decodeField.template operator()<Width>(ENCODING_WIDTH_WIDTH);
    const auto dest = decodeField.template operator()<uint32_t>(ENCODING_WIDTH_REGISTER_ADDRESS);

    std::array<RegisterOrImmediate, 2> sources {};
    size_t immediates_count = 0;
    for (size_t i = 0; i < sources.size(); ++i) {
      const auto register_address =
        decodeField.template operator()<uint32_t>(ENCODING_WIDTH_REGISTER_ADDRESS);
      const auto mode = decodeField.template operator()<SourceMode>(ENCODING_WIDTH_SOURCE_MODE);

      if (mode != SourceMode::Immediate) {
        sources[i].setRegisterAddress(register_address);
        continue;
      }

      const auto immediate = decodeImmediate(
        data.subspan(ENCODING_SIZE_MIN + immediates_count * ENCODING_SIZE_IMMEDIATE)
      );
      sources[i].setImmediateValue(immediate);
      ++immediates_count;
    }

    *this = { opcode, width, dest, sources };
  }

  bool InstructionData::isValid() const {
    if (!opcodeIsValid(m_opcode)) {
      return false;
    }

    if (opcodeGetType(m_opcode) == OpcodeType::Memory) {
      bool ok = false;
      switch (m_width) {
      case Width::Byte:
      case Width::HalfWord:
      case Width::Word:
        ok = true;
      }
      if (!ok) {
        return false;
      }
    } else if (m_width != Width::Word) {
      return false;
    }

    if (m_dest_register_addr >= RegisterBank::REGISTER_COUNT) {
      return false;
    }

    for (const auto& src : m_sources) {
      if (!src.isValid()) {
        return false;
      }
    }

    return true;
  }
}

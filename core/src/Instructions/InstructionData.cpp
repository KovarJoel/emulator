#include "InstructionData.hpp"

#include "Exceptions.hpp"
#include "RegisterBank.hpp"

namespace emulator::core::instructions {
  InstructionData::InstructionData()
    : m_opcode{ Opcode::ADD }, m_width{ Width::Word }, m_dest_register_addr{ 0 }, m_sources{} {
    // equivalent to NOP (ADD r0, r0, r0)
    if (!isValid()) {
      throw exceptions::BadInstructionValue{};
    }
  }

  InstructionData::InstructionData(Opcode opcode, Width width, uint32_t dest_register_addr, const std::array<RegisterOrImmediate, 2>& sources)
    : m_opcode{ opcode }, m_width{ width }, m_dest_register_addr{ dest_register_addr }, m_sources{ sources } {
    if (!isValid()) {
      throw exceptions::BadInstructionValue{};
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

  size_t InstructionData::getEncodingSize() const {
    if (opcodeGetType(m_opcode) == OpcodeType::Branch) {
      return ENCODING_SIZE_BRANCH;
    }
    else {
      size_t immediates_count = 0;
      for (const auto& src : m_sources) {
        if (src.getSourceMode() == RegisterOrImmediate::SourceMode::Immediate) {
          ++immediates_count;
        }
      }
      return ENCODING_SIZE_MIN + immediates_count * ENCODING_SIZE_IMMEDIATE;
    }
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

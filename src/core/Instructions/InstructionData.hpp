#pragma once

#include "Operations.hpp"

#include <array>
#include <cstdint>
#include <variant>

namespace core::instructions {
  class InstructionData {
  public:
    static constexpr std::size_t ENCODING_SIZE = 4;
    
    static constexpr std::size_t ENCODING_WIDTH_OPCODE = 8;
    static constexpr std::size_t ENCODING_WIDTH_REG_ADDR = 4;
    static constexpr std::size_t ENCODING_WIDTH_IMMEDIATE = 16;
    static constexpr std::size_t ENCODING_WIDTH_CONDITION_TYPE = 4;
    static constexpr std::size_t ENCODING_WIDTH_JUMP_TARGET = 20;
    static constexpr std::size_t ENCODING_WIDTH_JUMP_TARGET_OFFSET = 16;

    enum class ConditionType {
      BranchAlways,
      BranchNever,

      BranchIfEqual,
      BranchIfNotEqual,
      
      BranchIfGreater,
      BranchIfLess,
      BranchIfGreaterEqual,
      BranchIfLessEqual,

      BranchUnsignedIfGreater,
      BranchUnsignedIfLess,
      BranchUnsignedIfGreaterEqual,
      BranchUnsignedIfLessEqual,

      BranchCarry,
      BranchNotCarry,
      BranchOverflow,
      BranchNotOverflow
    };

    using RawData = std::array<std::uint8_t, ENCODING_SIZE>;
    
    using Opcode = Opcode;

    struct RTypeData {
      std::size_t dest_reg_addr;
      std::size_t src1_reg_addr;
      std::size_t src2_reg_addr;
    };

    struct ITypeData {
      std::size_t dest_reg_addr;
      std::size_t src1_reg_addr;
      std::int16_t immediate_value;
    };

    struct BTypeData {
      ConditionType branch_condition_type;
      std::int32_t relative_jump_target;
    };

    struct PTypeData {
      std::size_t link_reg_addr;
      std::size_t absolut_jump_target_reg_addr;
      std::int16_t relative_jump_target_offset;
    };

    struct JTypeData {
      std::size_t link_reg_addr;
      std::int32_t relative_jump_target;
    };

    struct ParsedData {
      std::variant<RTypeData, ITypeData, BTypeData, PTypeData, JTypeData> data;
      Opcode opcode;
    };

  public:
    InstructionData() = default;
    InstructionData(RawData raw_data)
      : m_raw_data{ raw_data } {}

    template <typename T>
    constexpr void encode(Opcode opcode, const T& instr_data) noexcept {
      m_raw_data = {};
      
      setOpcode(opcode);

      if constexpr (std::is_same_v<T, RTypeData>) {
        setDestRegister(instr_data.dest_reg_addr);
        setSrc1Register(instr_data.src1_reg_addr);
        setSrc2Register(instr_data.src2_reg_addr);
      } else if constexpr (std::is_same_v<T, ITypeData>) {
        setDestRegister(instr_data.dest_reg_addr);
        setSrc1Register(instr_data.src1_reg_addr);
        setImmediate(instr_data.immediate_value);
      } else if constexpr (std::is_same_v<T, BTypeData>) {
        setCondition(instr_data.branch_condition_type);
        setJumpTargetImmediate(instr_data.relative_jump_target);
      } else if constexpr (std::is_same_v<T, PTypeData>) {
        setLinkRegister(instr_data.link_reg_addr);
        setJumpTargetRegister(instr_data.absolut_jump_target_reg_addr);
        setJumpTargetOffset(instr_data.relative_jump_target_offset);
      } else if constexpr (std::is_same_v<T, JTypeData>) {
        setLinkRegister(instr_data.link_reg_addr);
        setJumpTargetImmediate(instr_data.relative_jump_target);
      } else {
        static_assert(false);
      }
    }

    constexpr void encode(const ParsedData& instr_data) noexcept {
      std::visit([&](auto&& data) { encode(instr_data.opcode, data); }, instr_data.data);
    }

    [[nodiscard]] constexpr ParsedData decode() const noexcept {
      ParsedData parsed{};
      parsed.opcode = getOpcode();
      switch (opcodeGetEncodingType(parsed.opcode)) {
        case EncodingType::R_TYPE:
          parsed.data = RTypeData{ getDestRegister(), getSrc1Register(), getSrc2Register() };
          break;
        case EncodingType::I_TYPE:
          parsed.data = ITypeData{ getDestRegister(), getSrc1Register(), getImmediate() };
          break;
        case EncodingType::B_TYPE:
          parsed.data = BTypeData{ getConditionType(), getJumpTargetImmediate() };
          break;
        case EncodingType::P_TYPE:
          parsed.data = PTypeData{ getLinkRegister(), getJumpTargetRegister(), getJumpTargetOffset() };
          break;
        case EncodingType::J_TYPE:
          parsed.data = JTypeData{ getLinkRegister(), getJumpTargetImmediate() };
          break;
      }
      return parsed;
    }

  private:
    [[nodiscard]] constexpr Opcode getOpcode() const noexcept {
      return static_cast<Opcode>(m_raw_data[0]);
    }
    
    [[nodiscard]] constexpr std::size_t getDestRegister() const noexcept {
      return m_raw_data[1] >> 4;
    }

    [[nodiscard]] constexpr std::size_t getSrc1Register() const noexcept {
      return m_raw_data[1] & 0x0f;
    }

    [[nodiscard]] constexpr std::size_t getSrc2Register() const noexcept {
      return m_raw_data[2] >> 4;
    }

    [[nodiscard]] constexpr std::int16_t getImmediate() const noexcept {
      return m_raw_data[2] | (m_raw_data[3] << 8);
    }

    [[nodiscard]] constexpr ConditionType getConditionType() const noexcept {
      return static_cast<ConditionType>(m_raw_data[1] >> 4);
    }

    [[nodiscard]] constexpr std::int32_t getJumpTargetImmediate() const noexcept {
      return (m_raw_data[1] & 0x0f) | (m_raw_data[2] << 4) | (m_raw_data[3] << 12);
    }

    [[nodiscard]] constexpr std::size_t getLinkRegister() const noexcept {
      return m_raw_data[1] >> 4;
    }

    [[nodiscard]] constexpr std::size_t getJumpTargetRegister() const noexcept {
      return m_raw_data[1] & 0x0f;
    }

    [[nodiscard]] constexpr std::int16_t getJumpTargetOffset() const noexcept {
      return m_raw_data[2] | (m_raw_data[3] << 8);
    }

    constexpr void setOpcode(Opcode opcode) noexcept {
      m_raw_data[0] = (m_raw_data[0] & 0b0000'0001) | (static_cast<std::uint8_t>(opcode) << 1);
    }

    constexpr void setDestRegister(std::size_t reg) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0x0f) | (static_cast<std::uint8_t>(reg) << 4);
    }

    constexpr void setSrc1Register(std::size_t reg) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0xf0) | static_cast<std::uint8_t>(reg);
    }

    constexpr void setSrc2Register(std::size_t reg) noexcept {
      m_raw_data[2] = (m_raw_data[2] & 0x0f) | (static_cast<std::uint8_t>(reg) << 4);
    }

    constexpr void setImmediate(std::int16_t immediate) noexcept {
      m_raw_data[2] = static_cast<std::uint8_t>(immediate);
      m_raw_data[3] = static_cast<std::uint8_t>(immediate >> 8);
    }

    constexpr void setCondition(ConditionType condition) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0x0f) | (static_cast<std::uint8_t>(condition) << 4);
    }

    constexpr void setJumpTargetImmediate(std::int32_t immediate) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0xf0) | (static_cast<std::uint8_t>(immediate) & 0x0f);
      m_raw_data[2] = static_cast<std::uint8_t>(immediate >> 4);
      m_raw_data[3] = static_cast<std::uint8_t>(immediate >> 12);
    }

    constexpr void setLinkRegister(std::size_t reg) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0x0f) | (static_cast<std::uint8_t>(reg) << 4);
    }

    constexpr void setJumpTargetRegister(std::size_t reg) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0xf0) | static_cast<std::uint8_t>(reg);
    }

    constexpr void setJumpTargetOffset(std::int16_t offset) noexcept {
      m_raw_data[2] = static_cast<std::uint8_t>(offset);
      m_raw_data[3] = static_cast<std::uint8_t>(offset >> 8);
    }

  private:
    RawData m_raw_data{};
  };
}

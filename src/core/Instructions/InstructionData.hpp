#pragma once

#include "core/Exceptions.hpp"

#include <array>
#include <cctype>
#include <cstdint>
#include <string_view>
#include <utility>
#include <variant>

// clang-format off
#define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(X)  \
  X(LD,       Load,                             =1)\
  X(LDH,      LoadHalfWord,                       )\
  X(LDHZX,    LoadHalfWordZeroExtend,             )\
  X(LDB,      LoadByte,                           )\
  X(LDBZX,    LoadByteZeroExtend,                 )\
  X(LDI,      LoadDisplaced,                      )\
  X(LDHI,     LoadHalfWordDisplaced,              )\
  X(LDHZXI,   LoadHalfWordZeroExtendDisplaced,    )\
  X(LDBI,     LoadByteDisplaced,                  )\
  X(LDBZXI,   LoadByteZeroExtendDisplaced,        )\
  X(ST,       Store,                              )\
  X(STH,      StoreHalfWord,                      )\
  X(STB,      StoreByte,                          )\
  X(STI,      StoreDisplaced,                     )\
  X(STHI,     StoreHalfWordDisplaced,             )\
  X(STBI,     StoreByteDisplaced,                 )\
  X(ADD,      Add,                                )\
  X(ADDI,     AddImmediate,                       )\
  X(SUB,      Subtract,                           )\
  X(SUBI,     SubtractImmediate,                  )\
  X(MUL,      Multiply,                           )\
  X(MULI,     MultiplyImmediate,                  )\
  X(DIV,      Divide,                             )\
  X(DIVI,     DivideImmediate,                    )\
  X(DIVU,     DivideUnsigned,                     )\
  X(DIVUI,    DivideUnsignedImmediate,            )\
  X(REM,      Remainder,                          )\
  X(REMI,     RemainderImmediate,                 )\
  X(REMU,     RemainderUnsigned,                  )\
  X(REMUI,    RemainderUnsignedImmediate,         )\
  X(ROL,      RotateLeft,                         )\
  X(ROLI,     RotateLeftImmediate,                )\
  X(ROR,      RotateRight,                        )\
  X(RORI,     RotateRightImmediate,               )\
  X(SLL,      ShiftLeftLogical,                   )\
  X(SLLI,     ShiftLeftLogicalImmediate,          )\
  X(SRL,      ShiftRightLogical,                  )\
  X(SRLI,     ShiftRightLogicalImmediate,         )\
  X(SRA,      ShiftRightArithmetic,               )\
  X(SRAI,     ShiftRightArithmeticImmediate,      )\
  X(AND,      And,                                )\
  X(ANDI,     AndImmediate,                       )\
  X(OR,       Or,                                 )\
  X(ORI,      OrImmediate,                        )\
  X(XOR,      Xor,                                )\
  X(XORI,     XorImmediate,                       )\
  X(JMPR,     JumpRegister,                       )\
  X(JMPI,     JumpImmediate,                      )\
  X(B,        Branch,                             )\
  X(HALT,     Halt,                               )\
  X(CYCL,     Cycle,                              )\
  X(RAND,     Random,                             )
// clang-format on

namespace core::instructions {
  enum class Opcode : uint8_t {
    #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM(mnemonic, class, init) mnemonic init,
      EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM)
    #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM
  };

  enum class EncodingType {
    R_TYPE, I_TYPE, B_TYPE, P_TYPE, J_TYPE
  };


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

  template <typename T>
  struct GetEncodingType;

  template <>
  struct GetEncodingType<RTypeData> {
    constexpr static EncodingType value = EncodingType::R_TYPE;
  };

  template <>
  struct GetEncodingType<ITypeData> {
    constexpr static EncodingType value = EncodingType::I_TYPE;
  };

  template <>
  struct GetEncodingType<BTypeData> {
    constexpr static EncodingType value = EncodingType::B_TYPE;
  };

  template <>
  struct GetEncodingType<PTypeData> {
    constexpr static EncodingType value = EncodingType::P_TYPE;
  };

  template <>
  struct GetEncodingType<JTypeData> {
    constexpr static EncodingType value = EncodingType::J_TYPE;
  };
  
  template <typename T>
  constexpr static EncodingType GetEncodingType_v = GetEncodingType<T>::value;
  
  EncodingType opcodeGetEncodingType(const Opcode& code);

  class InstructionData {
  public:
    static constexpr std::size_t ENCODING_SIZE = 4;
    
    static constexpr std::size_t ENCODING_WIDTH_OPCODE = 8;
    static constexpr std::size_t ENCODING_WIDTH_REG_ADDR = 4;
    static constexpr std::size_t ENCODING_WIDTH_IMMEDIATE = 16;
    static constexpr std::size_t ENCODING_WIDTH_CONDITION_TYPE = 4;
    static constexpr std::size_t ENCODING_WIDTH_JUMP_TARGET = 20;
    static constexpr std::size_t ENCODING_WIDTH_JUMP_TARGET_OFFSET = 16;

    using RawData = std::array<std::uint8_t, ENCODING_SIZE>;

    struct ParsedData {
      std::variant<RTypeData, ITypeData, BTypeData, PTypeData, JTypeData> data;
      Opcode opcode;
    };

  public:
    InstructionData() = default;
    InstructionData(RawData raw_data)
      : m_raw_data{ raw_data } {}
    InstructionData(const ParsedData& parsed_data)
      : m_raw_data{} {
      encode(parsed_data);
    }

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

    [[nodiscard]] constexpr ParsedData decode() const {
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

    [[nodiscard]] constexpr RawData getRaw() const noexcept {
      return m_raw_data;
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
      return static_cast<std::int16_t>(
        static_cast<std::uint16_t>(m_raw_data[2]) | static_cast<std::uint16_t>(m_raw_data[3] << 8)
      );
    }

    [[nodiscard]] constexpr ConditionType getConditionType() const noexcept {
      return static_cast<ConditionType>(m_raw_data[1] >> 4);
    }

    [[nodiscard]] constexpr std::int32_t getJumpTargetImmediate() const noexcept {
      const auto value = static_cast<std::uint32_t>(m_raw_data[1] & 0x0f) 
        | static_cast<std::uint32_t>((m_raw_data[2] << 4))
        | static_cast<std::uint32_t>(m_raw_data[3] << 12)
      ;

      struct signed20 {
        std::int32_t val : 20;
      };
      return static_cast<std::int32_t>(signed20{ static_cast<std::int32_t>(value) }.val);
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
      m_raw_data[0] = std::to_underlying(opcode);
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
      const auto value = static_cast<std::uint16_t>(immediate);
      m_raw_data[2] = static_cast<std::uint8_t>(value);
      m_raw_data[3] = static_cast<std::uint8_t>(value >> 8);
    }

    constexpr void setCondition(ConditionType condition) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0x0f) | (static_cast<std::uint8_t>(condition) << 4);
    }

    constexpr void setJumpTargetImmediate(std::int32_t immediate) noexcept {
      const auto value = static_cast<std::uint32_t>(immediate);
      m_raw_data[1] = (m_raw_data[1] & 0xf0) | (static_cast<std::uint8_t>(value) & 0x0f);
      m_raw_data[2] = static_cast<std::uint8_t>(value >> 4);
      m_raw_data[3] = static_cast<std::uint8_t>(value >> 12);
    }

    constexpr void setLinkRegister(std::size_t reg) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0x0f) | (static_cast<std::uint8_t>(reg) << 4);
    }

    constexpr void setJumpTargetRegister(std::size_t reg) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0xf0) | static_cast<std::uint8_t>(reg);
    }

    constexpr void setJumpTargetOffset(std::int16_t offset) noexcept {
      const auto value = static_cast<std::uint16_t>(offset);
      m_raw_data[2] = static_cast<std::uint8_t>(value);
      m_raw_data[3] = static_cast<std::uint8_t>(value >> 8);
    }

  private:
    RawData m_raw_data{};
  };

  [[nodiscard]] constexpr bool opcodeIsValid(Opcode code) noexcept {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, ...) case Opcode::mnemonic: return true;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    return false;
  }

  [[nodiscard]] constexpr std::string_view opcodeToString(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, ...) case Opcode::mnemonic: return #mnemonic;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }

  [[nodiscard]] constexpr Opcode opcodeFromString(std::string_view string) {
    const auto equals = [](std::string_view lhs, std::string_view rhs) {
      if (lhs.size() != rhs.size()) return false;
      for (size_t i = 0; i < lhs.size(); ++i) {
        if (std::tolower(lhs[i]) != std::tolower(rhs[i])) {
          return false;
        }
      }
      return true;
    };

    #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_IF(mnemonic, ...) if (equals(string, #mnemonic)) return Opcode::mnemonic;
      EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_IF)
    #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_IF
    throw exceptions::InvalidOperation{};
  }

  [[nodiscard]] constexpr std::string_view opcodeToName(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, class, ...) case Opcode::mnemonic: return #class;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }
}

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
    R_TYPE, I_TYPE, B_TYPE, J_TYPE
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
    std::size_t reg1_addr;
    std::size_t reg2_addr;
    std::size_t reg3_addr;
  };

  struct ITypeData {
    std::size_t reg1_addr;
    std::size_t reg2_addr;
    std::int16_t imm16;
  };

  struct BTypeData {
    ConditionType condition;
    std::int32_t imm20;
  };

  struct JTypeData {
    std::size_t reg1_addr;
    std::int32_t imm20;
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
      std::variant<RTypeData, ITypeData, BTypeData, JTypeData> data;
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
        setReg1(instr_data.reg1_addr);
        setReg2(instr_data.reg2_addr);
        setReg3(instr_data.reg3_addr);
      } else if constexpr (std::is_same_v<T, ITypeData>) {
        setReg1(instr_data.reg1_addr);
        setReg2(instr_data.reg2_addr);
        setImm16(instr_data.imm16);
      } else if constexpr (std::is_same_v<T, BTypeData>) {
        setCondition(instr_data.condition);
        setImm20(instr_data.imm20);
      } else if constexpr (std::is_same_v<T, JTypeData>) {
        setReg1(instr_data.reg1_addr);
        setImm20(instr_data.imm20);
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
          parsed.data = RTypeData{ getReg1(), getReg2(), getReg3() };
          break;
        case EncodingType::I_TYPE:
          parsed.data = ITypeData{ getReg1(), getReg2(), getImm16() };
          break;
        case EncodingType::B_TYPE:
          parsed.data = BTypeData{ getCondition(), getImm20() };
          break;
        case EncodingType::J_TYPE:
          parsed.data = JTypeData{ getReg1(), getImm20() };
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
    
    [[nodiscard]] constexpr std::size_t getReg1() const noexcept {
      return m_raw_data[1] >> 4;
    }

    [[nodiscard]] constexpr std::size_t getReg2() const noexcept {
      return m_raw_data[1] & 0x0f;
    }

    [[nodiscard]] constexpr std::size_t getReg3() const noexcept {
      return m_raw_data[2] >> 4;
    }

    [[nodiscard]] constexpr std::int16_t getImm16() const noexcept {
      return static_cast<std::int16_t>(
        static_cast<std::uint16_t>(m_raw_data[2]) | static_cast<std::uint16_t>(m_raw_data[3] << 8)
      );
    }

    [[nodiscard]] constexpr ConditionType getCondition() const noexcept {
      return static_cast<ConditionType>(m_raw_data[1] >> 4);
    }

    [[nodiscard]] constexpr std::int32_t getImm20() const noexcept {
      const auto value = static_cast<std::uint32_t>(m_raw_data[1] & 0x0f) 
        | static_cast<std::uint32_t>((m_raw_data[2] << 4))
        | static_cast<std::uint32_t>(m_raw_data[3] << 12)
      ;

      struct signed20 {
        std::int32_t val : 20;
      };
      return static_cast<std::int32_t>(signed20{ static_cast<std::int32_t>(value) }.val);
    }

    constexpr void setOpcode(Opcode opcode) noexcept {
      m_raw_data[0] = std::to_underlying(opcode);
    }

    constexpr void setReg1(std::size_t reg) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0x0f) | (static_cast<std::uint8_t>(reg) << 4);
    }

    constexpr void setReg2(std::size_t reg) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0xf0) | static_cast<std::uint8_t>(reg);
    }

    constexpr void setReg3(std::size_t reg) noexcept {
      m_raw_data[2] = (m_raw_data[2] & 0x0f) | (static_cast<std::uint8_t>(reg) << 4);
    }

    constexpr void setImm16(std::int16_t immediate) noexcept {
      const auto value = static_cast<std::uint16_t>(immediate);
      m_raw_data[2] = static_cast<std::uint8_t>(value);
      m_raw_data[3] = static_cast<std::uint8_t>(value >> 8);
    }

    constexpr void setCondition(ConditionType condition) noexcept {
      m_raw_data[1] = (m_raw_data[1] & 0x0f) | (static_cast<std::uint8_t>(condition) << 4);
    }

    constexpr void setImm20(std::int32_t immediate) noexcept {
      const auto value = static_cast<std::uint32_t>(immediate);
      m_raw_data[1] = (m_raw_data[1] & 0xf0) | (static_cast<std::uint8_t>(value) & 0x0f);
      m_raw_data[2] = static_cast<std::uint8_t>(value >> 4);
      m_raw_data[3] = static_cast<std::uint8_t>(value >> 12);
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

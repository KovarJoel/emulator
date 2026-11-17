#pragma once

#include "core/Exceptions.hpp"

#include <cctype>
#include <cstdint>
#include <string_view>

// clang-format off

// mnemonic, operation type,          encoding type,          operation class,  dest operands count, src operands count, init
#define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(X) \
  X(LD,     OpcodeType::Memory,       EncodingType::R_TYPE,   Load,                               1,  2,  =1)\
  X(LDH,    OpcodeType::Memory,       EncodingType::R_TYPE,   LoadHalfWord,                       1,  2,    )\
  X(LDHZX,  OpcodeType::Memory,       EncodingType::R_TYPE,   LoadHalfWordZeroExtend,             1,  2,    )\
  X(LDB,    OpcodeType::Memory,       EncodingType::R_TYPE,   LoadByte,                           1,  2,    )\
  X(LDBZX,  OpcodeType::Memory,       EncodingType::R_TYPE,   LoadByteZeroExtend,                 1,  2,    )\
  X(LDI,    OpcodeType::Memory,       EncodingType::I_TYPE,   LoadDisplaced,                      1,  2,    )\
  X(LDHI,   OpcodeType::Memory,       EncodingType::I_TYPE,   LoadHalfWordDisplaced,              1,  2,    )\
  X(LDHZXI, OpcodeType::Memory,       EncodingType::I_TYPE,   LoadHalfWordZeroExtendDisplaced,    1,  2,    )\
  X(LDBI,   OpcodeType::Memory,       EncodingType::I_TYPE,   LoadByteDisplaced,                  1,  2,    )\
  X(LDBZXI, OpcodeType::Memory,       EncodingType::I_TYPE,   LoadByteZeroExtendDisplaced,        1,  2,    )\
  X(ST,     OpcodeType::Memory,       EncodingType::R_TYPE,   Store,                              2,  1,    )\
  X(STH,    OpcodeType::Memory,       EncodingType::R_TYPE,   StoreHalfWord,                      2,  1,    )\
  X(STB,    OpcodeType::Memory,       EncodingType::R_TYPE,   StoreByte,                          2,  1,    )\
  X(STI,    OpcodeType::Memory,       EncodingType::I_TYPE,   StoreDisplaced,                     2,  1,    )\
  X(STHI,   OpcodeType::Memory,       EncodingType::I_TYPE,   StoreHalfWordDisplaced,             2,  1,    )\
  X(STBI,   OpcodeType::Memory,       EncodingType::I_TYPE,   StoreByteDisplaced,                 2,  1,    )\
  X(ADD,    OpcodeType::Arithmetic,   EncodingType::R_TYPE,   Add,                                1,  2,    )\
  X(ADDI,   OpcodeType::Arithmetic,   EncodingType::I_TYPE,   AddImmediate,                       1,  2,    )\
  X(SUB,    OpcodeType::Arithmetic,   EncodingType::R_TYPE,   Subtract,                           1,  2,    )\
  X(SUBI,   OpcodeType::Arithmetic,   EncodingType::I_TYPE,   SubtractImmediate,                  1,  2,    )\
  X(MUL,    OpcodeType::Arithmetic,   EncodingType::R_TYPE,   Multiply,                           1,  2,    )\
  X(MULI,   OpcodeType::Arithmetic,   EncodingType::I_TYPE,   MultiplyImmediate,                  1,  2,    )\
  X(DIV,    OpcodeType::Arithmetic,   EncodingType::R_TYPE,   Divide,                             1,  2,    )\
  X(DIVI,   OpcodeType::Arithmetic,   EncodingType::I_TYPE,   DivideImmediate,                    1,  2,    )\
  X(DIVU,   OpcodeType::Arithmetic,   EncodingType::R_TYPE,   DivideUnsigned,                     1,  2,    )\
  X(DIVUI,  OpcodeType::Arithmetic,   EncodingType::I_TYPE,   DivideUnsignedImmediate,            1,  2,    )\
  X(REM,    OpcodeType::Arithmetic,   EncodingType::R_TYPE,   Remainder,                          1,  2,    )\
  X(REMI,   OpcodeType::Arithmetic,   EncodingType::I_TYPE,   RemainderImmediate,                 1,  2,    )\
  X(REMU,   OpcodeType::Arithmetic,   EncodingType::R_TYPE,   RemainderUnsigned,                  1,  2,    )\
  X(REMUI,  OpcodeType::Arithmetic,   EncodingType::I_TYPE,   RemainderUnsignedImmediate,         1,  2,    )\
  X(ROL,    OpcodeType::Rotate,       EncodingType::R_TYPE,   RotateLeft,                         1,  2,    )\
  X(ROLI,   OpcodeType::Rotate,       EncodingType::I_TYPE,   RotateLeftImmediate,                1,  2,    )\
  X(ROR,    OpcodeType::Rotate,       EncodingType::R_TYPE,   RotateRight,                        1,  2,    )\
  X(RORI,   OpcodeType::Rotate,       EncodingType::I_TYPE,   RotateRightImmediate,               1,  2,    )\
  X(SLL,    OpcodeType::Shift,        EncodingType::R_TYPE,   ShiftLeftLogical,                   1,  2,    )\
  X(SLLI,   OpcodeType::Shift,        EncodingType::I_TYPE,   ShiftLeftLogicalImmediate,          1,  2,    )\
  X(SRL,    OpcodeType::Shift,        EncodingType::R_TYPE,   ShiftRightLogical,                  1,  2,    )\
  X(SRLI,   OpcodeType::Shift,        EncodingType::I_TYPE,   ShiftRightLogicalImmediate,         1,  2,    )\
  X(SRA,    OpcodeType::Shift,        EncodingType::R_TYPE,   ShiftRightArithmetic,               1,  2,    )\
  X(SRAI,   OpcodeType::Shift,        EncodingType::I_TYPE,   ShiftRightArithmeticImmediate,      1,  2,    )\
  X(AND,    OpcodeType::Logical,      EncodingType::R_TYPE,   And,                                1,  2,    )\
  X(ANDI,   OpcodeType::Logical,      EncodingType::I_TYPE,   AndImmediate,                       1,  2,    )\
  X(OR,     OpcodeType::Logical,      EncodingType::R_TYPE,   Or,                                 1,  2,    )\
  X(ORI,    OpcodeType::Logical,      EncodingType::I_TYPE,   OrImmediate,                        1,  2,    )\
  X(XOR,    OpcodeType::Logical,      EncodingType::R_TYPE,   Xor,                                1,  2,    )\
  X(XORI,   OpcodeType::Logical,      EncodingType::I_TYPE,   XorImmediate,                       1,  2,    )\
  X(JMPR,   OpcodeType::Jump,         EncodingType::P_TYPE,   JumpRegister,                       1,  2,    )\
  X(JMPI,   OpcodeType::Jump,         EncodingType::J_TYPE,   JumpImmediate,                      1,  1,    )\
  X(B,      OpcodeType::Branch,       EncodingType::B_TYPE,   BranchEqual,                        0,  1,    )\
  X(HALT,   OpcodeType::Other,        EncodingType::R_TYPE,   Halt,                               0,  0,    )\
  X(CYCL,   OpcodeType::Other,        EncodingType::R_TYPE,   Cycle,                              1,  0,    )\
  X(RAND,   OpcodeType::Other,        EncodingType::R_TYPE,   Random,                             1,  2,    )

namespace core::instructions {
  enum class OpcodeType {
    Memory,
    Arithmetic,
    Rotate,
    Shift,
    Logical,
    Jump,
    Branch,
    Other
  };

  enum class EncodingType {
    R_TYPE, I_TYPE, B_TYPE, P_TYPE, J_TYPE
  };

  enum class Opcode : uint8_t {
    #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM(mnemonic, type, encoding, class, dest, src, init) mnemonic init,
      EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM)
    #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM
  };

  [[nodiscard]] constexpr OpcodeType opcodeGetType(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, ...) case Opcode::mnemonic: return type;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }

  [[nodiscard]] constexpr EncodingType opcodeGetEncodingType(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, encoding, ...) case Opcode::mnemonic: return encoding;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }

  [[nodiscard]] constexpr size_t opcodeGetDestCount(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, encoding, class, dest, ...) case Opcode::mnemonic: return dest;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }  
  
  [[nodiscard]] constexpr size_t opcodeGetSrcCount(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, encoding, class, dest, src, ...) case Opcode::mnemonic: return src;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }

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
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, class, ...) case Opcode::mnemonic: return #class;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }
}

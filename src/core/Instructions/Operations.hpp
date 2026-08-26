#pragma once

#include "core/Exceptions.hpp"

#include <cctype>
#include <cstdint>
#include <string_view>

// clang-format off

// mnemonic, operation type, operation class, dest operands count, src operands count, init
#define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(X) \
  X(LD,     OpcodeType::Memory,       Load,                       1,  2,  =1)\
  X(LDZX,   OpcodeType::Memory,       LoadZeroExtend,             1,  2,    )\
  X(ST,     OpcodeType::Memory,       Store,                      2,  1,    )\
  X(ADD,    OpcodeType::Arithmetic,   Add,                        1,  2,    )\
  X(SUB,    OpcodeType::Arithmetic,   Subtract,                   1,  2,    )\
  X(MUL,    OpcodeType::Arithmetic,   Multiply,                   1,  2,    )\
  X(DIV,    OpcodeType::Arithmetic,   Divide,                     1,  2,    )\
  X(DIVU,   OpcodeType::Arithmetic,   DivideUnsigned,             1,  2,    )\
  X(REM,    OpcodeType::Arithmetic,   Remainder,                  1,  2,    )\
  X(REMU,   OpcodeType::Arithmetic,   RemainderUnsigned,          1,  2,    )\
  X(ROL,    OpcodeType::Rotate,       RotateLeft,                 1,  2,    )\
  X(ROR,    OpcodeType::Rotate,       RotateRight,                1,  2,    )\
  X(SLL,    OpcodeType::Shift,        ShiftLeftLogical,           1,  2,    )\
  X(SRL,    OpcodeType::Shift,        ShiftRightLogical,          1,  2,    )\
  X(SRA,    OpcodeType::Shift,        ShiftRightArithmetic,       1,  2,    )\
  X(AND,    OpcodeType::Logical,      And,                        1,  2,    )\
  X(OR,     OpcodeType::Logical,      Or,                         1,  2,    )\
  X(XOR,    OpcodeType::Logical,      Xor,                        1,  2,    )\
  X(JMP,    OpcodeType::Jump,         Jump,                       0,  1,    )\
  X(BE,     OpcodeType::Branch,       BranchEqual,                0,  1,    )\
  X(BNE,    OpcodeType::Branch,       BranchNotEqual,             0,  1,    )\
  X(BGT,    OpcodeType::Branch,       BranchGreater,              0,  1,    )\
  X(BLT,    OpcodeType::Branch,       BranchLess,                 0,  1,    )\
  X(BGE,    OpcodeType::Branch,       BranchGreaterEqual,         0,  1,    )\
  X(BLE,    OpcodeType::Branch,       BranchLessEqual,            0,  1,    )\
  X(BGTU,   OpcodeType::Branch,       BranchGreaterUnsigned,      0,  1,    )\
  X(BLTU,   OpcodeType::Branch,       BranchLessUnsigned,         0,  1,    )\
  X(BGEU,   OpcodeType::Branch,       BranchGreaterEqualUnsigned, 0,  1,    )\
  X(BLEU,   OpcodeType::Branch,       BranchLessEqualUnsigned,    0,  1,    )\
  X(BC,     OpcodeType::Branch,       BranchCarry,                0,  1,    )\
  X(BNC,    OpcodeType::Branch,       BranchNotCarry,             0,  1,    )\
  X(BO,     OpcodeType::Branch,       BranchOverflow,             0,  1,    )\
  X(BNO,    OpcodeType::Branch,       BranchNotOverflow,          0,  1,    )\
  X(HALT,   OpcodeType::Other,        Halt,                       0,  0,    )\
  X(CYCL,   OpcodeType::Other,        Cycle,                      1,  0,    )\
  X(RAND,   OpcodeType::Other,        Random,                     1,  2,    )

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

  enum class Opcode : uint8_t {
    #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM(mnemonic, type, class, dest, src, init) mnemonic init,
      EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM)
    #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM
  };

  constexpr OpcodeType opcodeGetType(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, ...) case Opcode::mnemonic: return type;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }

  constexpr size_t opcodeGetDestCount(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, class, dest, ...) case Opcode::mnemonic: return dest;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }  
  
  constexpr size_t opcodeGetSrcCount(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, class, dest, src, ...) case Opcode::mnemonic: return src;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }

  constexpr bool opcodeIsValid(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, ...) case Opcode::mnemonic: return true;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    return false;
  }

  constexpr std::string_view opcodeToString(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, ...) case Opcode::mnemonic: return #mnemonic;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }

  constexpr Opcode opcodeFromString(std::string_view string) {
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

  constexpr std::string_view opcodeToName(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, class, ...) case Opcode::mnemonic: return #class;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }
}

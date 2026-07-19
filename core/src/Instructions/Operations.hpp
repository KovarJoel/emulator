#pragma once

#include "Exceptions.hpp"

#include <cstdint>
#include <string_view>
#include <variant>

// mnemonic, operation type, operation class, init
#define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(X) \
  X(LD,     OpcodeType::Memory,       Load,                       =1)\
  X(LDZX,   OpcodeType::Memory,       LoadZeroExtend,               )\
  X(ST,     OpcodeType::Memory,       Store,                        )\
  X(ADD,    OpcodeType::Arithmetic,   Add,                          )\
  X(SUB,    OpcodeType::Arithmetic,   Subtract,                     )\
  X(MUL,    OpcodeType::Arithmetic,   Multiply,                     )\
  X(DIV,    OpcodeType::Arithmetic,   Divide,                       )\
  X(DIVU,   OpcodeType::Arithmetic,   DivideUnsigned,               )\
  X(REM,    OpcodeType::Arithmetic,   Remainder,                    )\
  X(REMU,   OpcodeType::Arithmetic,   RemainderUnsigned,            )\
  X(ROL,    OpcodeType::Rotate,       RotateLeft,                   )\
  X(ROR,    OpcodeType::Rotate,       RotateRight,                  )\
  X(SLL,    OpcodeType::Shift,        ShiftLeftLogical,             )\
  X(SRL,    OpcodeType::Shift,        ShiftRightLogical,            )\
  X(SRA,    OpcodeType::Shift,        ShiftRightArithmetic,         )\
  X(AND,    OpcodeType::Logical,      And,                          )\
  X(OR,     OpcodeType::Logical,      Or,                           )\
  X(XOR,    OpcodeType::Logical,      Xor,                          )\
  X(JMP,    OpcodeType::Jump,         Jump,                         )\
  X(BE,     OpcodeType::Branch,       BranchEqual,                  )\
  X(BNE,    OpcodeType::Branch,       BranchNotEqual,               )\
  X(BGT,    OpcodeType::Branch,       BranchGreater,                )\
  X(BLT,    OpcodeType::Branch,       BranchLess,                   )\
  X(BGE,    OpcodeType::Branch,       BranchGreaterEqual,           )\
  X(BLE,    OpcodeType::Branch,       BranchLessEqual,              )\
  X(BGTU,   OpcodeType::Branch,       BranchGreaterUnsigned,        )\
  X(BLTU,   OpcodeType::Branch,       BranchLessUnsigned,           )\
  X(BGEU,   OpcodeType::Branch,       BranchGreaterEqualUnsigned,   )\
  X(BLEU,   OpcodeType::Branch,       BranchLessEqualUnsigned,      )\
  X(BC,     OpcodeType::Branch,       BranchCarry,                  )\
  X(BNC,    OpcodeType::Branch,       BranchNotCarry,               )\
  X(BO,     OpcodeType::Branch,       BranchOverflow,               )\
  X(BNO,    OpcodeType::Branch,       BranchNotOverflow,            )\
  X(HALT,   OpcodeType::Other,        Halt,                         )\
  X(CYCL,   OpcodeType::Other,        Cycle,                        )

namespace emulator::core::instructions {
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
    #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_ENUM(mnemonic, type, class, init) mnemonic init,
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
  }

  constexpr Opcode opcodeFromString(std::string_view string) {
    #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_IF(mnemonic, ...) if (string == #mnemonic) return Opcode::mnemonic;
      EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_IF)
    #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_IF
  }

  constexpr std::string_view opcodeToName(Opcode code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, class, ...) case Opcode::mnemonic: return #class;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
  }
}

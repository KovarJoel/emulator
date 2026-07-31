#pragma once

#include "core/Exceptions.hpp"
#include "core/ProcessorState.hpp"
#include "InstructionData.hpp"
#include "Operations.hpp"

#include "Operations/Operation_Add.hpp"
#include "Operations/Operation_Branch.hpp"
#include "Operations/Operation_Cycle.hpp"
#include "Operations/Operation_Divide.hpp"
#include "Operations/Operation_Halt.hpp"
#include "Operations/Operation_Load.hpp"
#include "Operations/Operation_Logical.hpp"
#include "Operations/Operation_Multiply.hpp"
#include "Operations/Operation_Remainder.hpp"
#include "Operations/Operation_Rotate.hpp"
#include "Operations/Operation_Shift.hpp"
#include "Operations/Operation_Store.hpp"
#include "Operations/Operation_Subtract.hpp"

namespace core::instructions {
  namespace internal {
    using InstructionVariant = std::variant<
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CLASS(mnemonic, type, class, ...) operations::class,
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CLASS)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CLASS
      operations::bases::Base
    >;
    
    inline InstructionVariant opcodeToInstructionVariant(Opcode code) {
      switch (code) {
        #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, type, class, ...) case Opcode::mnemonic: return operations::class{};
          EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
        #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
      }
      throw exceptions::InvalidOperation{};
    }
  }

  class Instruction {
  public:
    Instruction() = default;
    Instruction(const InstructionData& data);
    void setData(const InstructionData& data);
    const InstructionData& getData() const;

    void execute(ProcessorState& state) const;

  private:
    InstructionData m_data{};
    internal::InstructionVariant m_instruction{};
  };
}

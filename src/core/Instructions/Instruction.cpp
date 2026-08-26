#include "Instruction.hpp"
#include "core/Exceptions.hpp"

namespace core::instructions {
  Instruction::Instruction(const InstructionData& data)
    : m_data { data }, m_instruction { internal::opcodeToInstructionVariant(m_data.getOpcode()) } {}

  void Instruction::execute(ProcessorState& state) const {
    try {
      std::visit([&](auto& operation) { operation.execute(m_data, state); }, m_instruction);
    } catch (const exceptions::Halt&) {
      ++state.cycle_count;
      throw;
    }
    ++state.cycle_count;
  }

  void Instruction::setData(const InstructionData& data) {
    m_data = data;
    m_instruction = internal::opcodeToInstructionVariant(m_data.getOpcode());
  }

  const InstructionData& Instruction::getData() const {
    return m_data;
  }
}

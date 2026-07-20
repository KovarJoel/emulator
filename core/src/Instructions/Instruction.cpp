#include "Instruction.hpp"

namespace emulator::core::instructions {
  void Instruction::execute(ProcessorState& state) const {
    std::visit([&](auto& operation) {
      operation.execute(m_data, state);
    }, m_instruction);
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

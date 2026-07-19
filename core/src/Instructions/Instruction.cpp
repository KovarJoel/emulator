#include "Instruction.hpp"

namespace emulator::core::instructions {
  void Instruction::execute(ProcessorState& state) const {
    std::visit([&](auto& operation) {
      operation.execute(m_data, state);
    }, m_instruction);
  }

  void Instruction::setData(const InstructionData& data) {
    m_data = data;
  }

  const InstructionData& Instruction::getData() const {
    return m_data;
  }
}

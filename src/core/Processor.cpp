#include "Processor.hpp"

#include "Exceptions.hpp"
#include "Instructions/Instruction.hpp"
#include "Instructions/InstructionData.hpp"

#include <mutex>

namespace core {
  void Processor::loadProgram(std::span<const std::byte> binary) {
    std::lock_guard<std::mutex> lock{ m_state.memory.getMutex() };
    m_state.memory.loadProgram(binary);
  }
  
  void Processor::loadProgram(const std::filesystem::path& file_path) {
    std::lock_guard<std::mutex> lock{ m_state.memory.getMutex() };
    m_state.memory.loadProgram(file_path);
  }

  void Processor::run(std::function<void(ProcessorState& state, const instructions::InstructionData& instruction)> callback) {
    initializeRegisters();

    instructions::Instruction instruction;

    try {
      while (true) {
        instruction.setData(fetchAndDecode());
        callback(m_state, instruction.getData());
        instruction.execute(m_state);
      }
    } catch(const exceptions::Halt& ex) {}
  }

  void Processor::initializeRegisters() {
    for (auto& reg : m_state.registers) {
      reg.set(0);
    }

    m_state.registers.getPC().set(m_state.memory.getHeader().entry_point);
    m_state.registers.getSP().set<uint32_t>(m_state.memory.RAM_SIZE);
  }

  instructions::InstructionData Processor::fetchAndDecode() const {
    std::lock_guard<std::mutex> lock{ m_state.memory.getMutex() };
    
    const auto pc = m_state.registers.getPC().get<uint32_t>();

    const auto code_begin = m_state.memory.getHeader().code_begin;
    const auto code_end = m_state.memory.getHeader().data_begin;
    
    if (pc < code_begin || pc >= code_end) {
      throw exceptions::InvalidExecuteAccess{};
    }

    instructions::InstructionData data;
    data.decode(m_state.memory.getRaw().subspan(pc, code_end - pc));
    return data;
  }
}

#include "Processor.hpp"

#include "Exceptions.hpp"
#include "Instructions/Instructions.hpp"

#include <mutex>

namespace core {
  void Processor::loadProgram(std::span<const std::byte> binary) {
    std::lock_guard<std::mutex> lock { m_state.memory.getMutex() };
    m_state.memory.loadProgram(binary);
  }

  void Processor::loadProgram(const std::filesystem::path& file_path) {
    std::lock_guard<std::mutex> lock { m_state.memory.getMutex() };
    m_state.memory.loadProgram(file_path);
  }

  void Processor::run(
    std::function<void(ProcessorState& state, const instructions::InstructionData::ParsedData& instruction)>
      callback
  ) {
    initializeRegisters();

    try {
      while (true) {
        const auto raw_data = fetchInstruction();
        const auto instruction_data = decodeInstruction(raw_data);

        callback(m_state, instruction_data);
        instructions::execute(instruction_data, m_state);
      }
    } catch ([[maybe_unused]] const exceptions::Halt& ex) {}
  }

  void Processor::initializeRegisters() {
    for (auto& reg : m_state.registers) {
      reg.set(0);
    }

    m_state.registers.getPC().set(m_state.memory.getHeader().entry_point);
    m_state.registers.getSP().set<uint32_t>(m_state.memory.RAM_SIZE);
  }

  instructions::InstructionData::RawData Processor::fetchInstruction() const {
    std::lock_guard<std::mutex> lock { m_state.memory.getMutex() };

    const auto pc = m_state.registers.getPC().get<uint32_t>();

    const auto code_begin = m_state.memory.getHeader().code_begin;
    const auto code_end = m_state.memory.getHeader().data_begin;

    if (pc < code_begin || pc >= code_end + instructions::InstructionData::ENCODING_SIZE) {
      throw exceptions::InvalidExecuteAccess {};
    }

    const auto raw_data = m_state.memory.getRaw().subspan(pc, instructions::InstructionData::ENCODING_SIZE);
    instructions::InstructionData::RawData raw_data_array{};
    std::copy(raw_data.begin(), raw_data.end(), raw_data_array.begin());
    return raw_data_array;
  }

  instructions::InstructionData::ParsedData Processor::decodeInstruction(instructions::InstructionData::RawData raw_data) const {
    instructions::InstructionData instruction_data{ raw_data };
    return instruction_data.decode();
  }
}

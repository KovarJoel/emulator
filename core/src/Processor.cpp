#include "Processor.hpp"

#include "Exceptions.hpp"
#include "Instructions/Instruction.hpp"

namespace emulator::core {
  void Processor::loadProgram(std::span<const std::byte> binary) {
    m_state.memory.loadProgram(binary);
  }
  
  void Processor::loadProgram(const std::filesystem::path& file_path) {
    m_state.memory.loadProgram(file_path);
  }

  void Processor::run() {
    initializeRegisters();

    instructions::Instruction instruction;

    try {
      while (true) {
        instruction.setData(fetchAndDecode());
        instruction.execute(m_state);
        ++m_state.cycle_count;
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
    using namespace instructions;
    using Width = InstructionData::Width;
    using SourceMode = RegisterOrImmediate::SourceMode;

    const auto pc = m_state.registers.getPC().get<uint32_t>();

    const auto code_begin = m_state.memory.getHeader().code_begin;
    const auto code_end = m_state.memory.getHeader().data_begin;

    if (pc < code_begin || pc + InstructionData::ENCODING_SIZE_MIN > code_end) {
      throw exceptions::InvalidExecuteAccess{};
    }

    const auto opcode_value = m_state.memory.get<std::underlying_type_t<Opcode>>(pc);
    const Opcode opcode = static_cast<Opcode>(opcode_value);
    if (!opcodeIsValid(opcode)) {
      throw exceptions::InvalidOperation{};
    }

    if (opcodeGetType(opcode) == OpcodeType::Branch) {
      if (pc + InstructionData::ENCODING_SIZE_BRANCH > code_end) {
        throw exceptions::InvalidExecuteAccess{};
      }
      const auto jump_target = fetchAndDecodeImmediate(m_state, pc + sizeof(Opcode));
      return InstructionData{ opcode, Width::Word, 0, { RegisterOrImmediate{ SourceMode::Immediate, jump_target } }};
    }

    const auto remaining_data = m_state.memory.get<uint16_t>(pc + sizeof(Opcode));
    size_t previous_width = 0;

    const Width width = decodeWidth(remaining_data);
    previous_width += InstructionData::ENCODING_WIDTH_WIDTH;

    const uint32_t dest = decodeRegisterAddress(remaining_data, previous_width);
    previous_width += InstructionData::ENCODING_WIDTH_REGISTER_ADDRESS;

    std::array<RegisterOrImmediate, 2> sources{};
    size_t immediates_count = 0;

    for (size_t i = 0; i < sources.size(); ++i) {
      const auto register_address = decodeRegisterAddress(remaining_data, previous_width);
      previous_width += InstructionData::ENCODING_WIDTH_REGISTER_ADDRESS;

      const auto mode = decodeSourceMode(remaining_data, previous_width);
      previous_width += InstructionData::ENCODING_WIDTH_SOURCE_MODE;

      if (mode != SourceMode::Immediate) {
        sources[i].setRegisterAddress(register_address);
        continue;
      }

      const auto address = pc + InstructionData::ENCODING_SIZE_MIN + immediates_count * InstructionData::ENCODING_SIZE_IMMEDIATE;
      if (address + InstructionData::ENCODING_SIZE_IMMEDIATE > code_end) {
        throw exceptions::InvalidExecuteAccess{};
      }
      
      const auto immediate = fetchAndDecodeImmediate(m_state, address);
      sources[i].setImmediateValue(immediate);

      ++immediates_count;
    }

    return InstructionData{ opcode, width, dest, sources };
  }

  uint32_t Processor::fetchAndDecodeImmediate(const ProcessorState& state, size_t address) {
    using namespace instructions;

    uint32_t immediate{};
    for (size_t i = 0; i < InstructionData::ENCODING_SIZE_IMMEDIATE; ++i) {
      immediate = immediate | (state.memory.get<uint8_t>(address + i) << (8 * i));
    }
    return immediate;
  }
  
  instructions::InstructionData::Width Processor::decodeWidth(uint16_t remaining_data) {
    using namespace instructions;
    using Width = InstructionData::Width;

    constexpr auto MASK = (1 << InstructionData::ENCODING_WIDTH_WIDTH) - 1;
    const Width width = static_cast<Width>(remaining_data & MASK);
    return width;
  }

  uint32_t Processor::decodeRegisterAddress(uint16_t remaining_data, size_t previous_width) {
    using namespace instructions;
    
    const auto mask = ((1 << InstructionData::ENCODING_WIDTH_REGISTER_ADDRESS) - 1) << previous_width;
    const uint32_t register_address = (remaining_data & mask) >> previous_width;
    return register_address;
  }

  instructions::RegisterOrImmediate::SourceMode Processor::decodeSourceMode(uint16_t remaining_data, size_t previous_width) {
    using namespace instructions;
    using SourceMode = RegisterOrImmediate::SourceMode;

    const auto mask = ((1 << InstructionData::ENCODING_WIDTH_SOURCE_MODE) - 1) << previous_width;
    const SourceMode mode = static_cast<SourceMode>((remaining_data & mask) >> previous_width);
    return mode;
  }
}

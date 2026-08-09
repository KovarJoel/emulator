#include "Encoder.hpp"

#include "core/Instructions/InstructionData.hpp"
#include "core/Instructions/Operations.hpp"
#include "core/Memory.hpp"

#include <cassert>
#include <cstdint>
#include <variant>

namespace assembler {
  const std::vector<std::byte>& Encoder::run(const IRGenerator::Data& data) {
    m_data.clear();
    m_ir_data = data;

    const auto instruction_data = createInstructionsData();
    
    const auto header = createHeader(instruction_data.size());
    const auto header_data = createHeaderData(header);

    const auto variable_data = createVariablesData(header);

    m_data.append_range(header_data);
    m_data.resize(header.code_begin - core::Memory::OFFSET_HEADER);
    m_data.append_range(instruction_data);
    m_data.resize(header.data_begin - core::Memory::OFFSET_HEADER);
    m_data.append_range(variable_data);

    return m_data;
  }

  std::vector<std::byte> Encoder::createInstructionsData() {
    using namespace core::instructions;

    std::vector<InstructionData> instructions{};

    uint32_t current_pc = core::Memory::OFFSET_CODE;

    // first pass for sizes
    for (auto& function : m_ir_data.functions) {
      function.address = current_pc;

      for (auto& inst : function.instructions) {
        for (auto& label : function.branches) {
          if (label.instruction_id == instructions.size()) {
            label.address = current_pc;
          }
        }

        instructions.emplace_back(
          inst.opcode,
          inst.width,
          createInstructionDestination(inst),
          createInstructionSources(function, inst)
        );

        current_pc += instructions.back().getEncodingSize();
      }
    }

    // second pass for labels
    for (auto& function : m_ir_data.functions) {
      for (auto& inst : function.instructions) {
        instructions.emplace_back(
          inst.opcode,
          inst.width,
          createInstructionDestination(inst),
          createInstructionSources(function, inst)
        );
      }
    }

    std::vector<std::byte> binary_instructions{};

    for (const auto& inst : instructions) {
      binary_instructions.append_range(inst.encode());
    }

    return binary_instructions;
  }

  uint32_t Encoder::createInstructionDestination(const IRGenerator::Instruction& inst) {
    if (inst.opcode == core::instructions::Opcode::ST) {
      return std::get<Parser::Register>(inst.operands.back()).address;
    }
    else if (core::instructions::opcodeGetDestCount(inst.opcode)) {
      return std::get<Parser::Register>(inst.operands.front()).address;
    }
    return 0;
  }

  std::array<core::instructions::RegisterOrImmediate, 2> Encoder::createInstructionSources(const IRGenerator::Function& func, const IRGenerator::Instruction& inst) {
    using namespace core::instructions;
    using enum RegisterOrImmediate::SourceMode;
    std::array<RegisterOrImmediate, 2> sources{};

    std::span<const IRGenerator::Operand> source_operands{};
    if (inst.opcode == core::instructions::Opcode::ST) {
      source_operands = { inst.operands.begin(), inst.operands.begin() + 2 };
    }
    else {
      source_operands = { inst.operands.begin() + opcodeGetDestCount(inst.opcode), inst.operands.end() };
    }

    for (size_t i = 0; i < source_operands.size(); ++i) {
      if (std::holds_alternative<Parser::Register>(source_operands[i])) {
        sources[i].setRegisterAddress(std::get<Parser::Register>(source_operands[i]).address);
      }
      else if (std::holds_alternative<Parser::Immediate>(source_operands[i])) {
        sources[i].setImmediateValue(std::get<Parser::Immediate>(source_operands[i]).value);
      }
      else if (std::holds_alternative<Parser::BranchTarget>(source_operands[i])) {
        const std::string& branch_name = std::get<Parser::BranchTarget>(source_operands[i]).label;

        const auto branch_it = std::find_if(func.branches.begin(), func.branches.end(), [&branch_name](const auto& branch) {
          return branch.name == branch_name;
        });

        if (branch_it == func.branches.end()) {
          const auto func_it = std::find_if(m_ir_data.functions.begin(), m_ir_data.functions.end(), [&branch_name](const auto& f) {
            return f.name == branch_name;
          });
          assert(func_it != m_ir_data.functions.end());

          sources[i].setImmediateValue(func_it->address);
        } else {
          sources[i].setImmediateValue(branch_it->address);
        }
      }
      else {
        assert(!"invalid variant alternative");
      }
    }

    return sources;
  }

  core::Memory::Header Encoder::createHeader(size_t code_size) {
    core::Memory::Header header{};
    
    uint32_t main_address = 0;
    for (const auto& func : m_ir_data.functions) {
      if (func.name == "main") {
        main_address = func.address;
        break;
      }
    }
    assert(main_address);

    const uint32_t data_size = calculateDataSize();

    header.code_begin = core::Memory::OFFSET_CODE;
    header.entry_point = main_address;
    header.data_begin = header.code_begin + code_size + core::Memory::PAGE_SIZE;
    header.data_begin = header.data_begin - header.data_begin % core::Memory::PAGE_SIZE;
    header.ram_begin = header.data_begin + data_size + core::Memory::PAGE_SIZE;
    header.ram_begin = header.ram_begin - header.ram_begin % core::Memory::PAGE_SIZE;

    return header;
  }

  uint32_t Encoder::calculateDataSize() const {
    uint32_t data_size = 0;
    
    for (const auto& var : m_ir_data.variables) {
      uint32_t element_size = 0;
      switch (var.width) {
        case IRGenerator::Instruction::Width::Byte:     element_size = 1; break;
        case IRGenerator::Instruction::Width::HalfWord: element_size = 2; break;
        case IRGenerator::Instruction::Width::Word:     element_size = 4; break;
      }

      data_size += element_size * var.initializer.size();
    }

    return data_size;
  }

  std::vector<std::byte> Encoder::createHeaderData(const core::Memory::Header& header) {
    std::vector<std::byte> data{};
    data.resize(sizeof header);
    std::memcpy(data.data(), &header, sizeof header);
    return data;
  }

  std::vector<std::byte> Encoder::createVariablesData(const core::Memory::Header& header) {
    std::vector<std::byte> data{};
    data.reserve(m_ir_data.variables.size() * 4);

    uint32_t address = header.data_begin;

    for (auto& var : m_ir_data.variables) {
      var.address = address;

      uint32_t element_size = 0;
      switch (var.width) {
        case IRGenerator::Instruction::Width::Byte:     element_size = 1; break;
        case IRGenerator::Instruction::Width::HalfWord: element_size = 2; break;
        case IRGenerator::Instruction::Width::Word:     element_size = 4; break;
      }
      
      for (const auto& value : var.initializer) {
        data.resize(data.size() + element_size); 
        std::memcpy(data.data() + (data.size() - element_size), &value, element_size);
      }

      address += element_size * var.initializer.size();
    }

    return data;
  }
}

#include "Encoder.hpp"

#include "core/Instructions/InstructionData.hpp"
#include "core/Memory.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <variant>

namespace assembler {
  const std::vector<std::byte>& Encoder::run(const IRGenerator::Data& data) {
    m_data.clear();
    m_ir_data = data;

    auto instruction_data = createInstructionsData();

    const auto header = createHeader(instruction_data.size());
    const auto header_data = createHeaderData(header);

    const auto variable_data = createVariablesData(header);

    instruction_data = secondPass();

    m_data.append_range(header_data);
    m_data.resize(header.code_begin - core::Memory::OFFSET_HEADER);
    m_data.append_range(instruction_data);
    m_data.resize(header.data_begin - core::Memory::OFFSET_HEADER);
    m_data.append_range(variable_data);

    return m_data;
  }

  std::vector<std::byte> Encoder::createInstructionsData() {
    using namespace core::instructions;

    std::vector<InstructionData> instructions {};

    uint32_t current_pc = core::Memory::OFFSET_CODE;
    size_t instructions_count = 0;

    // first pass for sizes
    for (auto& function : m_ir_data.functions) {
      function.address = current_pc;

      const size_t prev_funcs_instructions_count = instructions_count;

      for (auto& inst : function.instructions) {
        for (auto& label : function.branches) {
          if (label.instruction_id == instructions_count - prev_funcs_instructions_count) {
            label.address = current_pc;
          }
        }

        const auto parsed_data = createInstructionParsedData(function, inst);
        instructions.append_range(parsed_data);
        current_pc += parsed_data.size() * core::instructions::InstructionData::ENCODING_SIZE;
        ++instructions_count;

        // instructions.emplace_back(
        //   inst.opcode,
        //   inst.width,
        //   createInstructionDestination(inst),
        //   createInstructionSources(function, inst)
        // );

        // current_pc += instructions.back().getEncodingSize();
      }
    }

    std::vector<std::byte> binary_instructions {};

    for (const auto& inst : instructions) {
      const auto raw = inst.getRaw();
      for (auto& b : raw) {
        binary_instructions.emplace_back(static_cast<std::byte>(b));
      }
    }

    return binary_instructions;
  }

  std::vector<std::byte> Encoder::secondPass() {
    std::vector<std::byte> data {};

    for (auto& function : m_ir_data.functions) {
      for (auto& inst : function.instructions) {
        const auto expanded_inst = createInstructionParsedData(function, inst);
        for (auto& i : expanded_inst) {
          for (const auto& b : core::instructions::InstructionData{ i }.getRaw()) {
            data.push_back(static_cast<std::byte>(b));
          }
        }
        //data.append_range(
          //core::instructions::InstructionData { inst.opcode,
          //                                      inst.width,
          //                                      createInstructionDestination(inst),
          //                                      createInstructionSources(function, inst) }
          //  .encode()
        //);
      }
    }

    return data;
  }
  
  core::instructions::Opcode opcodeItoR(core::instructions::Opcode opcode) {
    using enum core::instructions::Opcode;
    switch (opcode) {
      case LDI: return LD;
      case LDHI: return LDH;
      case LDHZXI: return LDHZX;
      case LDBI: return LDB;
      case LDBZXI: return LDBZX;
      case STI: return ST;
      case STHI: return STH;
      case STBI: return STB;
      case ADDI: return ADD;
      case SUBI: return SUB;
      case MULI: return MUL;
      case DIVI: return DIV;
      case DIVUI: return DIVU;
      case REMI: return REM;
      case REMUI: return REMU;
      case ROLI: return ROL;
      case RORI: return ROR;
      case SLLI: return SLL;
      case SRLI: return SRL;
      case SRAI: return SRA;
      case ANDI: return AND;
      case ORI: return OR;
      case XORI: return XOR;
      //case JMPI: return JMPR;
      default:
      throw "Invalid opcode for opcodeItoR";
    }
  }

  const std::vector<core::instructions::InstructionData::ParsedData> Encoder::createInstructionParsedData(
    const IRGenerator::Function& func,
    const IRGenerator::Instruction& inst
  ) {
    using namespace core::instructions;
    using enum core::instructions::EncodingType;
    
    InstructionData::ParsedData data{};
    data.opcode = inst.opcode;

    switch (core::instructions::opcodeGetEncodingType(inst.opcode)) {
      case R_TYPE:
        data.data = RTypeData {
          .reg1_addr = std::get<Parser::Register>(inst.operands[0]).address,
          .reg2_addr = std::get<Parser::Register>(inst.operands[1]).address,
          .reg3_addr = std::get<Parser::Register>(inst.operands[2]).address
        };
        break;
      case I_TYPE:
        {
          auto immediate = getImmediateValue(func, inst.operands[2]);
          if (true || immediate < INT16_MIN || immediate > INT16_MAX) {
            switch (inst.opcode) {
              case Opcode::LDI:
              case Opcode::STI:
                immediate /= 4;
                break;
              case Opcode::LDHI:
              case Opcode::LDHZXI:
              case Opcode::STHI:
                immediate /= 2;
                break;
              case Opcode::LDBI:
              case Opcode::LDBZXI:
              case Opcode::STBI:
                immediate /= 1;
                break;
              default:
                break;
            }

            return {
              { ITypeData { 11, 0, static_cast<int16_t>(immediate >> 16) }, Opcode::ADDI },
              { ITypeData { 11, 11, 16 }, Opcode::SLLI },
              { ITypeData { 11, 11, static_cast<int16_t>(immediate & 0xFFFF) }, Opcode::ORI },
              { RTypeData {
                std::get<Parser::Register>(inst.operands[0]).address,
                std::get<Parser::Register>(inst.operands[1]).address,
                11
              }, opcodeItoR(inst.opcode) }
            };
          }
        }
        data.data = ITypeData {
          .reg1_addr = std::get<Parser::Register>(inst.operands[0]).address,
          .reg2_addr = std::get<Parser::Register>(inst.operands[1]).address,
          .imm16 = static_cast<std::int16_t>(getImmediateValue(func, inst.operands[2]))
        };
        break;
      case B_TYPE:
        {
          const auto immediate = getImmediateValue(func, inst.operands[1]);
          if (immediate > ((1 << 19) - 1)) {
            throw "immediate for b type to big";
          }
        }
        data.data = BTypeData {
          .condition = std::get<Parser::BranchCondition>(inst.operands[0]).condition,
          .imm20 = getImmediateValue(func, inst.operands[1])
        };
        break;
      case J_TYPE:
        {
          const auto immediate = getImmediateValue(func, inst.operands[1]);
          if (immediate > ((1 << 19) - 1)) {
            throw "immediate for b type to big";
          }
        }
        data.data = JTypeData {
          .reg1_addr = std::get<Parser::Register>(inst.operands[0]).address,
          .imm20 = getImmediateValue(func, inst.operands[1])
        };
        break;
    }

    return { data };
  }

  std::int32_t Encoder::getImmediateValue(
    const IRGenerator::Function& func,
    const IRGenerator::Operand& operand
  ) const {
    if (std::holds_alternative<Parser::Immediate>(operand)) {
      std::int32_t value = 0;
      std::visit([&](auto&& imm) { value = static_cast<std::int32_t>(imm); },
        std::get<Parser::Immediate>(operand).value
      );
      return value;
    } else if (std::holds_alternative<Parser::BranchTarget>(operand)) {
      const std::string& branch_name = std::get<Parser::BranchTarget>(operand).label;

      const auto branch_it =
        std::ranges::find_if(func.branches, [&branch_name](const auto& branch) {
          return branch.name == branch_name;
        });

      if (branch_it == func.branches.end()) {
        const auto func_it =
          std::ranges::find_if(m_ir_data.functions, [&branch_name](const auto& f) {
            return f.name == branch_name;
          });
        assert(func_it != m_ir_data.functions.end());
        
        return func_it->address;
      } else {
        return branch_it->address;
      }
    } else if (std::holds_alternative<Parser::VariableReference>(operand)) {
      const std::string& variable_name =
        std::get<Parser::VariableReference>(operand).name;

      const auto variable_it =
        std::ranges::find_if(m_ir_data.variables, [&variable_name](auto&& var) {
          return var.name == variable_name;
        });
      assert(variable_it != m_ir_data.variables.end());

      return variable_it->address;
    } else {
      assert(!"invalid variant alternative");
    }
  }
  
#if 0
  uint32_t Encoder::createInstructionDestination(const IRGenerator::Instruction& inst) {
    if (inst.opcode == core::instructions::Opcode::ST) {
      return std::get<Parser::Register>(inst.operands.back()).address;
    } else if (core::instructions::opcodeGetDestCount(inst.opcode)) {
      return std::get<Parser::Register>(inst.operands.front()).address;
    }
    return 0;
  }

  std::array<core::instructions::RegisterOrImmediate, 2> Encoder::createInstructionSources(
    const IRGenerator::Function& func,
    const IRGenerator::Instruction& inst
  ) {
    using namespace core::instructions;
    using enum RegisterOrImmediate::SourceMode;
    std::array<RegisterOrImmediate, 2> sources {};

    std::span<const IRGenerator::Operand> source_operands {};
    assert(
      inst.operands.size() == opcodeGetDestCount(inst.opcode) + opcodeGetSrcCount(inst.opcode)
    );
    if (inst.opcode == core::instructions::Opcode::ST) {
      source_operands = { inst.operands.begin(), inst.operands.begin() + 2 };
    } else {
      source_operands = { inst.operands.begin() + opcodeGetDestCount(inst.opcode),
                          inst.operands.end() };
    }

    for (size_t i = 0; i < source_operands.size(); ++i) {
      if (std::holds_alternative<Parser::Register>(source_operands[i])) {
        sources[i].setRegisterAddress(std::get<Parser::Register>(source_operands[i]).address);
      } else if (std::holds_alternative<Parser::Immediate>(source_operands[i])) {
        std::visit(
          [&sources, i](auto&& imm) { sources[i].setImmediateValue(static_cast<uint32_t>(imm)); },
          std::get<Parser::Immediate>(source_operands[i]).value
        );
      } else if (std::holds_alternative<Parser::BranchTarget>(source_operands[i])) {
        const std::string& branch_name = std::get<Parser::BranchTarget>(source_operands[i]).label;

        const auto branch_it =
          std::ranges::find_if(func.branches, [&branch_name](const auto& branch) {
            return branch.name == branch_name;
          });

        if (branch_it == func.branches.end()) {
          const auto func_it =
            std::ranges::find_if(m_ir_data.functions, [&branch_name](const auto& f) {
              return f.name == branch_name;
            });
          assert(func_it != m_ir_data.functions.end());

          sources[i].setImmediateValue(func_it->address);
        } else {
          sources[i].setImmediateValue(branch_it->address);
        }
      } else if (std::holds_alternative<Parser::VariableReference>(source_operands[i])) {
        const std::string& variable_name =
          std::get<Parser::VariableReference>(source_operands[i]).name;

        const auto variable_it =
          std::ranges::find_if(m_ir_data.variables, [&variable_name](auto&& var) {
            return var.name == variable_name;
          });
        assert(variable_it != m_ir_data.variables.end());

        sources[i].setImmediateValue(variable_it->address);
      } else {
        assert(!"invalid variant alternative");
      }
    }

    return sources;
  }
#endif

  core::Memory::Header Encoder::createHeader(size_t code_size) {
    core::Memory::Header header {};

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
    header.data_begin =
      header.code_begin + static_cast<uint32_t>(code_size) + core::Memory::PAGE_SIZE;
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
      case IRGenerator::Instruction::Width::Byte:
        element_size = 1;
        break;
      case IRGenerator::Instruction::Width::HalfWord:
        element_size = 2;
        break;
      case IRGenerator::Instruction::Width::Word:
        element_size = 4;
        break;
      }

      data_size += element_size * static_cast<uint32_t>(var.initializer.size());
    }

    return data_size;
  }

  std::vector<std::byte> Encoder::createHeaderData(const core::Memory::Header& header) {
    std::vector<std::byte> data {};
    data.resize(sizeof header);
    std::memcpy(data.data(), &header, sizeof header);
    return data;
  }

  std::vector<std::byte> Encoder::createVariablesData(const core::Memory::Header& header) {
    std::vector<std::byte> data {};
    data.reserve(m_ir_data.variables.size() * 4);

    uint32_t address = header.data_begin;

    for (auto& var : m_ir_data.variables) {
      var.address = address;

      uint32_t element_size = 0;
      switch (var.width) {
      case IRGenerator::Instruction::Width::Byte:
        element_size = 1;
        break;
      case IRGenerator::Instruction::Width::HalfWord:
        element_size = 2;
        break;
      case IRGenerator::Instruction::Width::Word:
        element_size = 4;
        break;
      }

      for (const auto& imm : var.initializer) {
        data.resize(data.size() + element_size);
        std::visit(
          [&](auto&& val) {
            std::memcpy(data.data() + (data.size() - element_size), &val, element_size);
          },
          imm.value
        );
      }

      address += element_size * static_cast<uint32_t>(var.initializer.size());
    }

    return data;
  }
}

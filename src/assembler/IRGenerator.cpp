#include "IRGenerator.hpp"

#include <cassert>
#include <variant>

namespace assembler {
  IRGenerator::IRGenerator(const AssemblerData& assembler_data)
    : m_assembler_data{ assembler_data } {}

  const IRGenerator::Data& IRGenerator::run(std::span<const Parser::Token> parser_tokens) {
    m_data = {};
    m_parser_tokens = parser_tokens;

    generateFunctions();
    generateVariables();

    return m_data;
  }

  void IRGenerator::generateFunctions() {
    for (size_t i = 0; i < m_parser_tokens.size(); ++i) {
      if (!std::holds_alternative<Parser::FunctionLabel>(m_parser_tokens[i].token)) {
        continue;
      }

      Function function{};
      function.name = std::get<Parser::FunctionLabel>(m_parser_tokens[i].token).label;
      ++i;

      while (true) {
        if (std::holds_alternative<Parser::BranchLabel>(m_parser_tokens[i].token)) {
          Branch branch{};
          branch.name = std::get<Parser::BranchLabel>(m_parser_tokens[i].token).label;
          branch.instruction_id = function.instructions.size();
          function.branches.push_back(std::move(branch));
          ++i;
        }
        else if (std::holds_alternative<Parser::Mnemonic>(m_parser_tokens[i].token)) {
          function.instructions.push_back(getInstruction(i));
        }
        else if (std::holds_alternative<Parser::NewLine>(m_parser_tokens[i].token)) {
          ++i;
        }
        else {
          break;
        }
      }

      m_data.functions.push_back(std::move(function));
    }
  }

  IRGenerator::Instruction IRGenerator::getInstruction(size_t& index) {
    Instruction instruction{};

    instruction.opcode = std::get<Parser::Mnemonic>(m_parser_tokens[index].token).opcode;
    ++index;

    if (std::holds_alternative<Parser::WidthSpecifier>(m_parser_tokens[index].token)) {
      instruction.width = std::get<Parser::WidthSpecifier>(m_parser_tokens[index].token).width;
      ++index;
    }
    else {
      instruction.width = Instruction::Width::Word;
    }

    instruction.operands = getOperands(index);

    return instruction;
  }

  std::vector<IRGenerator::Operand> IRGenerator::getOperands(size_t& index) {
    std::vector<IRGenerator::Operand> operands{};
    
    while (true) {
      if (std::holds_alternative<Parser::Register>(m_parser_tokens[index].token)) {
        operands.push_back(std::get<Parser::Register>(m_parser_tokens[index].token));
      }
      else if (std::holds_alternative<Parser::Immediate>(m_parser_tokens[index].token)) {
        operands.push_back(std::get<Parser::Immediate>(m_parser_tokens[index].token));
      }
      else if (std::holds_alternative<Parser::BranchTarget>(m_parser_tokens[index].token)) {
        operands.push_back(std::get<Parser::BranchTarget>(m_parser_tokens[index].token));
      }
      else {
        break;
      }
      ++index;
    }

    return operands;
  }

  void IRGenerator::generateVariables() {
    for (size_t i = 0; i < m_parser_tokens.size(); ++i) {
      if (!std::holds_alternative<Parser::VariableName>(m_parser_tokens[i].token)) {
        continue;
      }

      Variable variable{};
      variable.name = std::get<Parser::VariableName>(m_parser_tokens[i].token).name;
      ++i;

      if (std::holds_alternative<Parser::WidthSpecifier>(m_parser_tokens[i].token)) {
        variable.width = std::get<Parser::WidthSpecifier>(m_parser_tokens[i].token).width;
        ++i;
      }
      else {
        variable.width = Variable::Width::Word;
      }

      while (std::holds_alternative<Parser::Immediate>(m_parser_tokens[i].token)) {
        variable.initializer.push_back(std::get<Parser::Immediate>(m_parser_tokens[i].token).value);
        ++i;
      }

      m_data.variables.push_back(std::move(variable));
    }
  }
}

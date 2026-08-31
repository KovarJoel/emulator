#include "IRGenerator.hpp"
#include "assembler/AssemblerError.hpp"
#include "core/Instructions/InstructionData.hpp"

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <variant>

namespace assembler {
  IRGenerator::IRGenerator(const AssemblerData& assembler_data)
    : m_assembler_data { assembler_data } {}

  const IRGenerator::Data& IRGenerator::run(std::span<const Parser::Token> parser_tokens) {
    m_data = {};
    m_function_names.clear();
    m_branches.clear();
    m_branch_targets.clear();
    m_parser_tokens = parser_tokens;

    generateFunctions();
    generateVariables();

    validateBranches();
    validateVariables();

    return m_data;
  }

  void IRGenerator::generateFunctions() {
    for (size_t i = 0; i < m_parser_tokens.size(); ++i) {
      if (!std::holds_alternative<Parser::FunctionLabel>(m_parser_tokens[i].token)) {
        continue;
      }

      const size_t function_label_index = i;

      Function function {};
      function.name = std::get<Parser::FunctionLabel>(m_parser_tokens[i].token).label;
      updateFunctionNames(m_parser_tokens[i]);
      ++i;

      size_t last_branch_index {};
      while (true) {
        if (std::holds_alternative<Parser::BranchLabel>(m_parser_tokens[i].token)) {
          Branch branch {};
          branch.name = std::get<Parser::BranchLabel>(m_parser_tokens[i].token).label;
          m_branches[function.name].push_back(m_parser_tokens[i]);
          branch.instruction_id = static_cast<uint32_t>(function.instructions.size());

          last_branch_index = i;

          function.branches.push_back(std::move(branch));
          ++i;
        } else if (std::holds_alternative<Parser::Mnemonic>(m_parser_tokens[i].token)) {
          function.instructions.push_back(getInstruction(function, i));
        } else if (std::holds_alternative<Parser::NewLine>(m_parser_tokens[i].token)) {
          ++i;
        } else {
          --i;
          break;
        }
      }

      if (
        !function.branches.empty()
        && function.branches.back().instruction_id == function.instructions.size()
      )
      {
        throw Error { ErrorType::IRGenerator_VoidBranch,
                      m_assembler_data.input_file_path,
                      m_assembler_data.lines[m_parser_tokens[last_branch_index].line],
                      m_parser_tokens[last_branch_index].line + 1,
                      m_parser_tokens[last_branch_index].column + 1 };
      }

      if (function.instructions.empty()) {
        throw Error { ErrorType::IRGenerator_EmptyFunction,
                      m_assembler_data.input_file_path,
                      m_assembler_data.lines[m_parser_tokens[function_label_index].line],
                      m_parser_tokens[function_label_index].line + 1,
                      m_parser_tokens[function_label_index].column + 1 };
      }

      m_data.functions.push_back(std::move(function));
    }

    if (!m_function_names.contains("main")) {
      throw Error { ErrorType::IRGenerator_MissingMain, m_assembler_data.input_file_path };
    }
  }

  void IRGenerator::updateFunctionNames(const Parser::Token& token) {
    const auto& new_name = std::get<Parser::FunctionLabel>(token.token).label;

    if (m_function_names.contains(new_name)) {
      throw Error { ErrorType::IRGenerator_DuplicateFunction,
                    m_assembler_data.input_file_path,
                    m_assembler_data.lines[token.line],
                    token.line + 1,
                    token.column + 1 };
    }

    m_function_names.insert(new_name);
  }

  IRGenerator::Instruction IRGenerator::getInstruction(const Function& function, size_t& index) {
    Instruction instruction {};

    instruction.opcode = std::get<Parser::Mnemonic>(m_parser_tokens[index].token).opcode;
    ++index;

    if (std::holds_alternative<Parser::WidthSpecifier>(m_parser_tokens[index].token)) {
      instruction.width = std::get<Parser::WidthSpecifier>(m_parser_tokens[index].token).width;
      ++index;
    } else {
      instruction.width = Instruction::Width::Word;
    }

    instruction.operands = getOperands(function, index);

    return instruction;
  }

  std::vector<IRGenerator::Operand> IRGenerator::getOperands(
    const Function& function,
    size_t& index
  ) {
    std::vector<IRGenerator::Operand> operands {};

    while (true) {
      if (std::holds_alternative<Parser::Register>(m_parser_tokens[index].token)) {
        operands.push_back(std::get<Parser::Register>(m_parser_tokens[index].token));
      } else if (std::holds_alternative<Parser::Immediate>(m_parser_tokens[index].token)) {
        operands.push_back(std::get<Parser::Immediate>(m_parser_tokens[index].token));
      } else if (std::holds_alternative<Parser::BranchTarget>(m_parser_tokens[index].token)) {
        operands.push_back(std::get<Parser::BranchTarget>(m_parser_tokens[index].token));
        m_branch_targets[function.name].push_back(m_parser_tokens[index]);
      } else if (std::holds_alternative<Parser::VariableReference>(m_parser_tokens[index].token)) {
        operands.push_back(std::get<Parser::VariableReference>(m_parser_tokens[index].token));
      } else {
        break;
      }
      ++index;
    }

    return operands;
  }

  void IRGenerator::generateVariables() {
    for (size_t i = 0; i < m_parser_tokens.size(); ++i) {
      if (!std::holds_alternative<Parser::VariableDefinition>(m_parser_tokens[i].token)) {
        continue;
      }

      Variable variable {};
      variable.name = std::get<Parser::VariableDefinition>(m_parser_tokens[i].token).name;
      ++i;

      if (std::holds_alternative<Parser::WidthSpecifier>(m_parser_tokens[i].token)) {
        variable.width = std::get<Parser::WidthSpecifier>(m_parser_tokens[i].token).width;
        ++i;
      } else {
        variable.width = Variable::Width::Word;
      }

      while (std::holds_alternative<Parser::Immediate>(m_parser_tokens[i].token)) {
        variable.initializer.push_back(std::get<Parser::Immediate>(m_parser_tokens[i].token));
        ++i;
      }

      m_data.variables.push_back(std::move(variable));
    }
  }

  void IRGenerator::validateBranches() {
    for (const auto& [func_name, branches] : m_branches) {
      for (const auto& branch_token : branches) {
        const auto& label = std::get<Parser::BranchLabel>(branch_token.token).label;
        if (m_function_names.contains(label)) {
          throw Error { ErrorType::IRGenerator_ShaddowingBranch,
                        m_assembler_data.input_file_path,
                        m_assembler_data.lines[branch_token.line],
                        branch_token.line + 1,
                        branch_token.column + 1 };
        }
      }
    }

    for (const auto& [func_name, branch_targets] : m_branch_targets) {
      for (const auto& branch_token : branch_targets) {
        const auto& label = std::get<Parser::BranchTarget>(branch_token.token).label;

        if (
          !m_function_names.contains(label)
          && !std::ranges::contains(
            m_branches[func_name],
            label,
            [](const auto& tk) { return std::get<Parser::BranchLabel>(tk.token).label; }
          )
        )
        {
          throw Error { ErrorType::IRGenerator_UndefinedBranch,
                        m_assembler_data.input_file_path,
                        m_assembler_data.lines[branch_token.line],
                        branch_token.line + 1,
                        branch_token.column + 1 };
        }
      }
    }
  }

  void IRGenerator::validateVariables() {
    for (const auto& func : m_data.functions) {
      for (const auto& inst : func.instructions) {
        for (const auto& operand : inst.operands) {
          if (!std::holds_alternative<Parser::VariableReference>(operand)) {
            continue;
          }

          const auto& variable_name = std::get<Parser::VariableReference>(operand).name;
          if (!std::ranges::contains(
                m_data.variables,
                variable_name,
                [](auto&& var) { return var.name; }
              ))
          {
            assert(!"undefined variable, implement better diagnostics");
            throw Error {
              ErrorType::IRGenerator_UndefinedVariable,
              m_assembler_data.input_file_path,
            };
          }
        }
      }
    }

    for (const auto& var : m_data.variables) {
      for (const auto& immediate : var.initializer) {
        std::visit(
          [&var](auto val) {
            if constexpr (std::is_signed_v<decltype(val)>) {
              const auto [min, max] = getMinMax<int32_t>(widthToBits(var.width));
              validateImmediate(min, max, val);
            } else {
              const auto [min, max] = getMinMax<uint32_t>(widthToBits(var.width));
              validateImmediate(min, max, val);
            }
          },
          immediate.value
        );
      }
    }

    for (const auto& func : m_data.functions) {
      for (const auto& inst : func.instructions) {
        for (const auto& operand : inst.operands) {
          if (!std::holds_alternative<Parser::Immediate>(operand)) {
            continue;
          }

          const auto& immediate = std::get<Parser::Immediate>(operand).value;

          std::visit(
            [&](auto val) {
              if constexpr (std::is_signed_v<decltype(val)>) {
                const auto [min, max] =
                  getMinMax<int32_t>(core::instructions::InstructionData::ENCODING_WIDTH_IMMEDIATE);
                validateImmediate(min, max, val);
              } else {
                const auto [min, max] = getMinMax<uint32_t>(
                  core::instructions::InstructionData::ENCODING_WIDTH_IMMEDIATE
                );
                validateImmediate(0u, static_cast<uint32_t>(max), val);
              }
            },
            immediate
          );
        }
      }
    }
  }
}

#pragma once

#include "AssemblerData.hpp"
#include "Parser.hpp"
#include "core/Instructions/Operations.hpp"

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace assembler {
  class IRGenerator {
  public:
    struct Variable {
      using Width = Parser::WidthSpecifier::Width;

      std::string name{};
      uint32_t address{};
      Width width{};
      std::vector<uint32_t> initializer{};
    };

    struct Branch {
      std::string name{};
      uint32_t instruction_id{};
      uint32_t address{};
    };

    using Operand = std::variant<Parser::Register, Parser::Immediate, Parser::BranchTarget>;

    struct Instruction {
      using Opcode = core::instructions::Opcode;
      using Width = Parser::WidthSpecifier::Width;

      Opcode opcode;
      Width width;
      std::vector<Operand> operands;
    };

    struct Function {
      std::string name;
      uint32_t address;
      std::vector<Branch> branches;
      std::vector<Instruction> instructions;
    };

    struct Data {
      std::vector<Function> functions;
      std::vector<Variable> variables;
    };
    
  public:
    IRGenerator(const AssemblerData& assembler_data);

    const Data& run(std::span<const Parser::Token> parser_tokens);

  public:
    void generateFunctions();
    void generateVariables();

  private:
    std::vector<Operand> getOperands(size_t& index);
    Instruction getInstruction(size_t& index);

  private:
    const AssemblerData& m_assembler_data;

    std::span<const Parser::Token> m_parser_tokens{};
    Data m_data{};
  };
}

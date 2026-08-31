#pragma once

#include "AssemblerData.hpp"
#include "core/Instructions/Operations.hpp"
#include "Parser.hpp"

#include <cassert>
#include <climits>
#include <concepts>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace assembler {
  class IRGenerator {
  public:
    struct Variable {
      using Width = Parser::WidthSpecifier::Width;

      std::string name {};
      uint32_t address {};
      Width width {};
      std::vector<Parser::Immediate> initializer {};
    };

    struct Branch {
      std::string name {};
      size_t instruction_id {};
      uint32_t address {};
    };

    using Operand = std::
      variant<Parser::Register, Parser::Immediate, Parser::BranchTarget, Parser::VariableReference>;

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

    void updateFunctionNames(const Parser::Token& token);
    void validateBranches();
    void validateVariables();

  private:
    std::vector<Operand> getOperands(const Function& function, size_t& index);
    Instruction getInstruction(const Function& function, size_t& index);

    template <std::integral T, std::integral U>
    requires(std::is_signed_v<T> == std::is_signed_v<U> && sizeof(T) >= sizeof(U))
    static void validateImmediate(T min, T max, U immediate) {
      if (immediate < min || immediate > max) {
        throw Error { ErrorType::IRGenerator_InsufficientWidthForImmediate };
      }
    }

    template <std::integral T>
    requires(sizeof(T) == 4)
    static constexpr std::pair<T, T> getMinMax(size_t bits) {
      if constexpr (std::is_unsigned_v<T>) {
        return { static_cast<T>(0), static_cast<T>((1ull << bits) - 1) };
      }

      return { static_cast<T>(-(1ll << (bits - 1))), static_cast<T>((1ll << (bits - 1)) - 1) };
    }

    static constexpr uint32_t widthToBits(Variable::Width width) {
      switch (width) {
      case Variable::Width::Byte:
        return 1 * CHAR_BIT;
      case Variable::Width::HalfWord:
        return 2 * CHAR_BIT;
      case Variable::Width::Word:
        return 4 * CHAR_BIT;
      }
      assert(!"invalid width");
    }

  private:
    const AssemblerData& m_assembler_data;

    std::span<const Parser::Token> m_parser_tokens {};
    Data m_data {};

    std::unordered_set<std::string> m_function_names {};

    std::unordered_map<std::string, std::vector<Parser::Token>> m_branches {};
    std::unordered_map<std::string, std::vector<Parser::Token>> m_branch_targets {};
  };
}

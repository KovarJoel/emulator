#pragma once

#include "Lexer.hpp"

#include "AssemblerData.hpp"
#include "AssemblerError.hpp"

#include "core/Instructions/InstructionData.hpp"
#include "core/Instructions/Operations.hpp"

#include <cstddef>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace assembler {
  class Parser {
  public:
    struct Segment {
      enum class Type { None, Code, Data } type { Type::None };
    };

    struct FunctionLabel {
      std::string label;
    };

    struct BranchLabel {
      std::string label;
    };

    struct BranchTarget {
      std::string label;
    };

    struct VariableReference {
      std::string name;
    };

    struct VariableDefinition {
      std::string name;
    };

    struct Mnemonic {
      core::instructions::Opcode opcode;
    };

    struct WidthSpecifier {
      using Width = core::instructions::InstructionData::Width;
      Width width;
    };

    struct Register {
      uint32_t address;
    };

    struct Immediate {
      uint32_t value;
    };

    struct NewLine {};

    struct EndOfFile {};

    using TokenVariant = std::variant<
      Segment,
      FunctionLabel,
      BranchLabel,
      BranchTarget,
      VariableReference,
      VariableDefinition,
      Mnemonic,
      WidthSpecifier,
      Register,
      Immediate,
      NewLine,
      EndOfFile>;

    struct Token {
      TokenVariant token;
      size_t line;
      size_t column;

      friend std::ostream& operator<<(std::ostream& out, const Token& token);
    };

  public:
    Parser(const AssemblerData& assembler_data);

    const std::vector<Token>& run(std::span<const Lexer::Token> lexer_tokens);

  private:
    static constexpr const char* SEGMENT = "segment";
    static constexpr const char* SEGMENT_CODE = "code";
    static constexpr const char* SEGMENT_DATA = "data";
    static constexpr const char* FUNCTION = "function";

  private:
    void parseSegmentLine();
    void parseFunctionLine();
    void parseBranch();
    void parseVariableLine();
    void parseInstructionLine();

    void parseInitializerLine();
    void parseOperandsLine(core::instructions::Opcode opcode);
    void parseMemoryTransferOperands();
    void parseRegisterOperand();
    void parseOperandSeparator();
    void parseWidthSpecifier();

    void parseStoreOperandsLine();
    void parseLoadOperandsLine();
    void parseJumpOperandsLine();
    void parseBranchOperandsLine();

    void parseImmediate();

    void parseNumber();
    void parseString();
    void parseNewLine();

  private:
    static std::string lower(std::string str);
    static bool isMnemonic(const Lexer::Token& token);
    Error createError(ErrorType type, size_t line_index = -1, size_t column_index = -1) const;

  private:
    const AssemblerData& m_assembler_data;
    std::span<const Lexer::Token> m_lexer_tokens {};

    std::vector<Token> m_tokens {};
    std::span<const Lexer::Token> m_remaining_lexer_tokens {};

    Segment m_current_segment { Parser::Segment::Type::None };
  };
}

#include "Parser.hpp"

#include "assembler/AssemblerError.hpp"
#include "assembler/Lexer.hpp"
#include "core/Exceptions.hpp"
#include "core/Instructions/Operations.hpp"
#include "core/RegisterBank.hpp"
#include "Utility.hpp"

#include <cassert>
#include <charconv>
#include <stdexcept>
#include <string>
#include <variant>

namespace assembler {
  std::ostream& operator<<(std::ostream& out, const Parser::Token& token) {
    const auto& tk = token.token;
    if (std::holds_alternative<Parser::Segment>(tk)) {
      std::println(
        out,
        "Segment:            {}",
        std::get<Parser::Segment>(tk).type == Parser::Segment::Type::Code ? "Code" : "Data"
      );
    } else if (std::holds_alternative<Parser::FunctionLabel>(tk)) {
      std::println(out, "FunctionLabel:      {}", std::get<Parser::FunctionLabel>(tk).label);
    } else if (std::holds_alternative<Parser::BranchLabel>(tk)) {
      std::println(out, "BranchLabel:        {}", std::get<Parser::BranchLabel>(tk).label);
    } else if (std::holds_alternative<Parser::VariableReference>(tk)) {
      std::println(out, "VariableReference:  {}", std::get<Parser::VariableReference>(tk).name);
    } else if (std::holds_alternative<Parser::VariableDefinition>(tk)) {
      std::println(out, "VariableDefinition: {}", std::get<Parser::VariableDefinition>(tk).name);
    } else if (std::holds_alternative<Parser::Mnemonic>(tk)) {
      std::println(
        out,
        "Mnemonic:           {}",
        core::instructions::opcodeToName(std::get<Parser::Mnemonic>(tk).opcode)
      );
    } else if (std::holds_alternative<Parser::WidthSpecifier>(tk)) {
      using Width = Parser::WidthSpecifier::Width;
      const auto width = std::get<Parser::WidthSpecifier>(tk).width;
      std::println(
        out,
        "WidthSpecifier:     {}",
        width == Width::Byte ? "Byte" : (width == Width::HalfWord ? "HalfWord" : "Word")
      );
    } else if (std::holds_alternative<Parser::Register>(tk)) {
      std::println(out, "Register:           R{}", std::get<Parser::Register>(tk).address);
    } else if (std::holds_alternative<Parser::Immediate>(tk)) {
      const auto& value = std::get<Parser::Immediate>(tk).value;
      std::println(out, "Immediate:          {} (0x{:x})", value, value);
    } else if (std::holds_alternative<Parser::NewLine>(tk)) {
      std::println(out, "NewLine");
    } else if (std::holds_alternative<Parser::EndOfFile>(tk)) {
      std::println(out, "EndOfFile");
    }

    return out;
  }

  Parser::Parser(const AssemblerData& assembler_data)
    : m_assembler_data { assembler_data } {}

  std::string Parser::lower(std::string str) {
    for (auto& ch : str) {
      ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return str;
  }

  bool Parser::isMnemonic(const Lexer::Token& token) {
    if (token.type != Lexer::TokenType::Identifier) {
      return false;
    }

    try {
      core::instructions::opcodeFromString(token.value);
    } catch (const core::exceptions::InvalidOperation&) {
      return false;
    }

    return true;
  }

  Error Parser::createError(ErrorType type, size_t line_index, size_t column_index) const {
    return Error { type,
                   m_assembler_data.input_file_path,
                   m_assembler_data.lines[line_index],
                   line_index + 1,
                   column_index + 1 };
  }

  void Parser::parseString() {
    assert(!m_remaining_lexer_tokens.empty());

    const auto& lexer_token = m_remaining_lexer_tokens[0];
    if (lexer_token.type != Lexer::TokenType::String) {
      throw createError(ErrorType::Parser_ExpectedString, lexer_token.line, lexer_token.column);
    }

    if (lexer_token.value.empty()) {
      throw createError(ErrorType::Parser_EmptyString, lexer_token.line, lexer_token.column);
    }

    for (size_t i = 0; i < lexer_token.value.size(); ++i) {
      const uint32_t value = static_cast<unsigned char>(lexer_token.value[i]);
      m_tokens.emplace_back(Immediate { value }, lexer_token.line, lexer_token.column + i);
    }

    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);
  }

  void Parser::parseNumber() {
    assert(!m_remaining_lexer_tokens.empty());

    bool is_signed = false;

    if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::Number) {
      is_signed = false;
    } else if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::Minus) {
      is_signed = true;
    } else {
      throw createError(
        ErrorType::Parser_ExpectedNumber,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    if (is_signed) {
      assert(m_remaining_lexer_tokens.size() >= 2);
      if (m_remaining_lexer_tokens[1].type != Lexer::TokenType::Number) {
        throw createError(
          ErrorType::Parser_ExpectedNumberAfterMinus,
          m_remaining_lexer_tokens[1].line,
          m_remaining_lexer_tokens[1].column
        );
      }
    }

    const auto& number_token = m_remaining_lexer_tokens[is_signed ? 1 : 0];
    assert(!number_token.value.empty());

    char base_letter = 'd';
    std::string string_value {};

    if (is_signed) {
      string_value = "-";
    }

    if (
      number_token.value.size() >= 2
      && number_token.value[0] == '0'
      && isBaseSymbol(number_token.value[1])
    )
    {
      base_letter = number_token.value[1];
      string_value.append_range(number_token.value.substr(2));
    } else {
      string_value.append_range(number_token.value);
    }

    int base {};
    switch (base_letter) {
    case 'x':
      base = 16;
      break;
    case 'd':
      base = 10;
      break;
    case 'o':
      base = 8;
      break;
    case 'b':
      base = 2;
      break;
    default:
      assert(!"[Parser] Invalid number base");
    }

    std::from_chars_result conversion_result {};
    uint32_t result {};

    if (is_signed) {
      int32_t value {};
      conversion_result = std::from_chars(
        string_value.data(),
        string_value.data() + string_value.size(),
        value,
        base
      );
      result = static_cast<uint32_t>(value);
    } else {
      conversion_result = std::from_chars(
        string_value.data(),
        string_value.data() + string_value.size(),
        result,
        base
      );
    }

    if (conversion_result.ec == std::errc::result_out_of_range) {
      throw createError(
        ErrorType::Parser_NumericValueOutOfRange,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    assert(
      conversion_result.ec == std::errc {}
      && conversion_result.ptr == string_value.data() + string_value.size()
    );

    m_tokens.emplace_back(
      Immediate { result },
      m_remaining_lexer_tokens[0].line,
      m_remaining_lexer_tokens[0].column
    );
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(is_signed ? 2 : 1);
  }

  void Parser::parseImmediate() {
    assert(!m_remaining_lexer_tokens.empty());
    if (
      m_remaining_lexer_tokens[0].type == Lexer::TokenType::Minus
      || m_remaining_lexer_tokens[0].type == Lexer::TokenType::Number
    )
    {
      parseNumber();
    } else if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::Dollar) {
      assert(m_remaining_lexer_tokens.size() >= 2);
      if (m_remaining_lexer_tokens[1].type != Lexer::TokenType::Identifier) {
        throw createError(
          ErrorType::Parser_ExpectedVariableName,
          m_remaining_lexer_tokens[1].line,
          m_remaining_lexer_tokens[1].column
        );
      }
      m_tokens.emplace_back(
        VariableReference { m_remaining_lexer_tokens[1].value },
        m_remaining_lexer_tokens[1].line,
        m_remaining_lexer_tokens[1].column
      );
      m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(2);
    } else if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::String) {
      if (m_remaining_lexer_tokens[0].value.size() != 1) {
        throw createError(
          ErrorType::Parser_ExpectedSizeOneCharImmediate,
          m_remaining_lexer_tokens[0].line,
          m_remaining_lexer_tokens[0].column
        );
      }
      parseString();
    } else {
      throw createError(
        ErrorType::Parser_ExpectedImmediate,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
  }

  void Parser::parseNewLine() {
    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::NewLine) {
      throw createError(
        ErrorType::Parser_ExpectedEndOfLine,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
    m_tokens.emplace_back(
      NewLine {},
      m_remaining_lexer_tokens[0].line,
      m_remaining_lexer_tokens[0].column
    );
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);
  }

  void Parser::parseOperandSeparator() {
    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::Comma) {
      throw createError(
        ErrorType::Parser_ExpectedOperandSeparator,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);
  }

  void Parser::parseRegisterOperand() {
    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::Identifier) {
      throw createError(
        ErrorType::Parser_ExpectedRegisterOperand,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    const auto register_name = lower(m_remaining_lexer_tokens[0].value);

    if (!register_name.starts_with('r')) {
      throw createError(
        ErrorType::Parser_ExpectedRegisterOperand,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    const auto register_address_string = register_name.substr(1);
    unsigned long register_address = 0;
    try {
      register_address = std::stoul(register_address_string);
    } catch (const std::invalid_argument&) {
      throw createError(
        ErrorType::Parser_ExpectedRegisterOperand,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    } catch (const std::out_of_range&) {
      throw createError(
        ErrorType::Parser_InvalidRegisterAddress,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    if (std::to_string(register_address) != register_address_string) {
      throw createError(
        ErrorType::Parser_ExpectedRegisterOperand,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
    if (register_address >= core::RegisterBank::REGISTER_COUNT) {
      throw createError(
        ErrorType::Parser_InvalidRegisterAddress,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    m_tokens.emplace_back(
      Register { static_cast<uint32_t>(register_address) },
      m_remaining_lexer_tokens[0].line,
      m_remaining_lexer_tokens[0].column
    );
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);
  }

  void Parser::parseMemoryTransferOperands() {
    assert(!m_remaining_lexer_tokens.empty());

    bool uses_displacement = false;

    const Parser::Token zero_displacement { Immediate { 0 },
                                            m_remaining_lexer_tokens[0].line,
                                            m_remaining_lexer_tokens[0].column };

    if (
      m_remaining_lexer_tokens[0].type == Lexer::TokenType::Minus
      || m_remaining_lexer_tokens[0].type == Lexer::TokenType::Number
      || m_remaining_lexer_tokens[0].type == Lexer::TokenType::Dollar
    )
    {
      parseImmediate();
      uses_displacement = true;
    }

    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::LeftParen) {
      throw createError(
        ErrorType::Parser_ExpectedAddressingMode,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);

    parseRegisterOperand();

    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::RightParen) {
      throw createError(
        ErrorType::Parser_ExpectedAddressingMode,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);

    if (uses_displacement) {
      return;
    }

    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::LeftSquare) {
      m_tokens.insert(std::prev(m_tokens.end()), zero_displacement);
      return;
    }
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);

    parseRegisterOperand();

    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::RightSquare) {
      throw createError(
        ErrorType::Parser_ExpectedAddressingMode,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);
  }

  void Parser::parseLoadOperandsLine() {
    parseRegisterOperand();
    parseOperandSeparator();
    parseMemoryTransferOperands();
    parseNewLine();
  }

  void Parser::parseStoreOperandsLine() {
    parseMemoryTransferOperands();
    parseOperandSeparator();
    parseRegisterOperand();
    parseNewLine();
  }

  void Parser::parseJumpOperandsLine() {
    assert(!m_remaining_lexer_tokens.empty());

    if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::Number) {
      parseNumber();
    } else if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::Identifier) {
      try {
        parseRegisterOperand();
      } catch (const Error&) {
        m_tokens.emplace_back(
          BranchTarget { m_remaining_lexer_tokens[0].value },
          m_remaining_lexer_tokens[0].line,
          m_remaining_lexer_tokens[0].column
        );
        m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);
      }
    } else {
      throw createError(
        ErrorType::Parser_ExpectedJumpTarget,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    parseNewLine();
  }

  void Parser::parseBranchOperandsLine() {
    assert(!m_remaining_lexer_tokens.empty());

    if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::Number) {
      parseNumber();
    } else if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::Identifier) {
      m_tokens.emplace_back(
        BranchTarget { m_remaining_lexer_tokens[0].value },
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
      m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);
    } else {
      throw createError(
        ErrorType::Parser_ExpectedBranchTarget,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    parseNewLine();
  }

  void Parser::parseOperandsLine(core::instructions::Opcode opcode) {
    using namespace core::instructions;
    if (opcode == Opcode::LDZX || opcode == Opcode::LD) {
      parseLoadOperandsLine();
      return;
    } else if (opcode == Opcode::ST) {
      parseStoreOperandsLine();
      return;
    } else if (opcodeGetType(opcode) == OpcodeType::Jump) {
      parseJumpOperandsLine();
      return;
    } else if (opcodeGetType(opcode) == OpcodeType::Branch) {
      parseBranchOperandsLine();
      return;
    }

    const size_t dest_count = opcodeGetDestCount(opcode);
    const size_t src_count = opcodeGetSrcCount(opcode);

    for (size_t i = 0; i < dest_count; ++i) {
      parseRegisterOperand();
      if (i + 1 < dest_count || src_count) {
        parseOperandSeparator();
      }
    }

    for (size_t i = 0; i < src_count; ++i) {
      assert(!m_remaining_lexer_tokens.empty());

      if (
        m_remaining_lexer_tokens[0].type == Lexer::TokenType::Minus
        || m_remaining_lexer_tokens[0].type == Lexer::TokenType::Number
        || m_remaining_lexer_tokens[0].type == Lexer::TokenType::Dollar
        || m_remaining_lexer_tokens[0].type == Lexer::TokenType::String
      )
      {
        parseImmediate();
      } else if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::Identifier) {
        parseRegisterOperand();
      } else {
        throw createError(
          ErrorType::Parser_ExpectedOperand,
          m_remaining_lexer_tokens[0].line,
          m_remaining_lexer_tokens[0].column
        );
      }

      if (i + 1 < src_count) {
        parseOperandSeparator();
      }
    }

    parseNewLine();
  }

  const std::vector<Parser::Token>& Parser::run(std::span<const Lexer::Token> lexer_tokens) {
    m_tokens.clear();
    m_current_segment.type = Parser::Segment::Type::None;

    m_lexer_tokens = lexer_tokens;
    m_remaining_lexer_tokens = m_lexer_tokens;

    while (true) {
      assert(!m_remaining_lexer_tokens.empty());
      const auto& front = m_remaining_lexer_tokens.front();

      // 6 types of lines are possible
      // - empty lines                    NewLine
      // - segment declarations           Identifier(segment)
      // code:
      // - function label declarations    Identifier(function)
      // - branch label declarations      Identifier, Colon
      // - instructions                   Identifier, optional Dot+Identifier,
      // Identifier/Minus/Number/NewLine data:
      // - variable assignment            Identifier, optional Dot+Identifier, Equal

      if (front.type == Lexer::TokenType::NewLine) {
        parseNewLine();
        continue;
      } else if (front.type == Lexer::TokenType::EndOfFile) {
        assert(m_remaining_lexer_tokens.size() == 1);
        m_tokens.emplace_back(EndOfFile {}, front.line, front.column);
        m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);
        break;
      } else if (front.type != Lexer::TokenType::Identifier) {
        throw createError(ErrorType::Parser_UnexpectedLineStart, front.line, front.column);
      }

      assert(m_remaining_lexer_tokens.size() >= 2);
      if (lower(front.value) == SEGMENT) {
        parseSegmentLine();
      } else if (m_current_segment.type == Segment::Type::Code) {
        if (lower(front.value) == FUNCTION) {
          parseFunctionLine();
        } else if (m_remaining_lexer_tokens[1].type == Lexer::TokenType::Colon) {
          parseBranch();
        } else {
          parseInstructionLine();
        }
      } else if (m_current_segment.type == Segment::Type::Data) {
        parseVariableLine();
      } else {
        throw createError(ErrorType::Parser_MissingSegment, front.line, front.column);
      }
    }

    return m_tokens;
  }

  void Parser::parseInstructionLine() {
    assert(!m_remaining_lexer_tokens.empty());

    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::Identifier) {
      throw createError(
        ErrorType::Parser_ExpectedMnemonic,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
    if (!isMnemonic(m_remaining_lexer_tokens[0])) {
      throw createError(
        ErrorType::Parser_UnknownMnemonic,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    using namespace core::instructions;
    const auto opcode = opcodeFromString(m_remaining_lexer_tokens[0].value);
    m_tokens.emplace_back(
      Mnemonic { opcode },
      m_remaining_lexer_tokens[0].line,
      m_remaining_lexer_tokens[0].column
    );
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);

    assert(!m_remaining_lexer_tokens.empty());
    if (
      opcodeGetType(opcode) == OpcodeType::Memory
      && m_remaining_lexer_tokens[0].type == Lexer::TokenType::Dot
    )
    {
      parseWidthSpecifier();
    }

    parseOperandsLine(opcode);
  }

  void Parser::parseInitializerLine() {
    assert(!m_remaining_lexer_tokens.empty());

    const auto old_lexer_tokens = m_remaining_lexer_tokens;

    bool expect_number = true;
    bool valid_end = false;

    while (!m_remaining_lexer_tokens.empty()) {
      const auto& front = m_remaining_lexer_tokens.front();

      if (expect_number) {
        expect_number = false;
        valid_end = true;

        if (front.type == Lexer::TokenType::Number || front.type == Lexer::TokenType::Minus) {
          parseNumber();
        } else if (front.type == Lexer::TokenType::String) {
          parseString();
        } else {
          throw createError(ErrorType::Parser_ExpectedInitializerValue, front.line, front.column);
        }
      } else {
        if (front.type == Lexer::TokenType::Comma) {
          expect_number = true;
          valid_end = false;
          parseOperandSeparator();
        } else if (front.type == Lexer::TokenType::NewLine) {
          expect_number = false;
          valid_end = true;
          parseNewLine();
          break;
        } else {
          throw createError(ErrorType::Parser_ExpectedEndOfInitializer, front.line, front.column);
        }
      }
    }

    const auto& last_used =
      old_lexer_tokens[old_lexer_tokens.size() - m_remaining_lexer_tokens.size()];

    if (!valid_end) {
      throw createError(
        ErrorType::Parser_ExpectedInitializerValue,
        last_used.line,
        last_used.column
      );
    }
    if (!std::holds_alternative<NewLine>(m_tokens.back().token)) {
      throw createError(ErrorType::Parser_ExpectedEndOfLine, last_used.line, last_used.column);
    }
  }

  void Parser::parseWidthSpecifier() {
    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::Dot) {
      throw createError(
        ErrorType::Parser_ExpectedWidthSpecifier,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
    assert(m_remaining_lexer_tokens.size() >= 2);

    if (m_remaining_lexer_tokens[1].type != Lexer::TokenType::Identifier) {
      throw createError(
        ErrorType::Parser_ExpectedWidthSpecifier,
        m_remaining_lexer_tokens[2].line,
        m_remaining_lexer_tokens[2].column
      );
    }

    if (lower(m_remaining_lexer_tokens[1].value) == "b") {
      m_tokens.emplace_back(
        WidthSpecifier { WidthSpecifier::Width::Byte },
        m_remaining_lexer_tokens[1].line,
        m_remaining_lexer_tokens[1].column
      );
      m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(2);
    } else if (lower(m_remaining_lexer_tokens[1].value) == "h") {
      m_tokens.emplace_back(
        WidthSpecifier { WidthSpecifier::Width::HalfWord },
        m_remaining_lexer_tokens[1].line,
        m_remaining_lexer_tokens[1].column
      );
      m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(2);
    } else if (lower(m_remaining_lexer_tokens[1].value) == "w") {
      m_tokens.emplace_back(
        WidthSpecifier { WidthSpecifier::Width::Word },
        m_remaining_lexer_tokens[1].line,
        m_remaining_lexer_tokens[1].column
      );
      m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(2);
    } else {
      throw createError(
        ErrorType::Parser_InvalidWidthSpecifier,
        m_remaining_lexer_tokens[1].line,
        m_remaining_lexer_tokens[1].column
      );
    }
  }

  void Parser::parseVariableLine() {
    assert(!m_remaining_lexer_tokens.empty());

    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::Identifier) {
      throw createError(
        ErrorType::Parser_ExpectedVariableName,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    m_tokens.emplace_back(
      VariableDefinition { m_remaining_lexer_tokens[0].value },
      m_remaining_lexer_tokens[0].line,
      m_remaining_lexer_tokens[0].column
    );
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);

    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type == Lexer::TokenType::Dot) {
      parseWidthSpecifier();
    }

    assert(!m_remaining_lexer_tokens.empty());
    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::Equal) {
      throw createError(
        ErrorType::Parser_ExpectedEquals,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(1);

    parseInitializerLine();
  }

  void Parser::parseBranch() {
    assert(m_remaining_lexer_tokens[1].type == Lexer::TokenType::Colon);

    if (m_remaining_lexer_tokens[0].type != Lexer::TokenType::Identifier) {
      throw createError(
        ErrorType::Parser_ExpectedBranchLabel,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    m_tokens.emplace_back(
      BranchLabel { m_remaining_lexer_tokens[0].value },
      m_remaining_lexer_tokens[0].line,
      m_remaining_lexer_tokens[0].column
    );
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(2);
  }

  void Parser::parseFunctionLine() {
    assert(!m_remaining_lexer_tokens.empty());
    if (
      m_remaining_lexer_tokens[0].type != Lexer::TokenType::Identifier
      || lower(m_remaining_lexer_tokens[0].value) != FUNCTION
    )
    {
      throw createError(
        ErrorType::Parser_ExpectedFunctionDeclaration,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    assert(m_remaining_lexer_tokens.size() >= 2);
    if (m_remaining_lexer_tokens[1].type != Lexer::TokenType::Identifier) {
      throw createError(
        ErrorType::Parser_MissingFunctionName,
        m_remaining_lexer_tokens[1].line,
        m_remaining_lexer_tokens[1].column
      );
    }

    assert(m_remaining_lexer_tokens.size() >= 3);
    if (m_remaining_lexer_tokens[2].type != Lexer::TokenType::Colon) {
      throw createError(
        ErrorType::Parser_MissingFunctionColon,
        m_remaining_lexer_tokens[2].line,
        m_remaining_lexer_tokens[2].column
      );
    }

    m_tokens.emplace_back(
      FunctionLabel { m_remaining_lexer_tokens[1].value },
      m_remaining_lexer_tokens[0].line,
      m_remaining_lexer_tokens[0].column
    );
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(3);

    parseNewLine();
  }

  void Parser::parseSegmentLine() {
    assert(!m_remaining_lexer_tokens.empty());
    if (
      m_remaining_lexer_tokens[0].type != Lexer::TokenType::Identifier
      || lower(m_remaining_lexer_tokens[0].value) != SEGMENT
    )
    {
      throw createError(
        ErrorType::Parser_ExpectedSegment,
        m_remaining_lexer_tokens[0].line,
        m_remaining_lexer_tokens[0].column
      );
    }

    assert(m_remaining_lexer_tokens.size() >= 2);
    if (m_remaining_lexer_tokens[1].type != Lexer::TokenType::Colon) {
      throw createError(
        ErrorType::Parser_MissingSegmentColon,
        m_remaining_lexer_tokens[1].line,
        m_remaining_lexer_tokens[1].column
      );
    }

    assert(m_remaining_lexer_tokens.size() >= 3);
    if (m_remaining_lexer_tokens[2].type != Lexer::TokenType::Identifier ||
      (lower(m_remaining_lexer_tokens[2].value) != SEGMENT_CODE && lower(m_remaining_lexer_tokens[2].value) != SEGMENT_DATA))
    {
      throw createError(
        ErrorType::Parser_MissingSegmentType,
        m_remaining_lexer_tokens[2].line,
        m_remaining_lexer_tokens[2].column
      );
    }

    m_tokens.emplace_back(
      Segment { lower(m_remaining_lexer_tokens[2].value) == SEGMENT_CODE ? Segment::Type::Code
                                                                         : Segment::Type::Data },
      m_remaining_lexer_tokens[0].line,
      m_remaining_lexer_tokens[0].column
    );
    m_current_segment = std::get<Segment>(m_tokens.back().token);
    m_remaining_lexer_tokens = m_remaining_lexer_tokens.subspan(3);

    parseNewLine();
  }
}

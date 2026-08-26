#include "Lexer.hpp"

#include "assembler/AssemblerError.hpp"
#include "Utility.hpp"

#include <cassert>
#include <cctype>
#include <print>

namespace assembler {
  std::ostream& operator<<(std::ostream& out, const Lexer::Token& token) {
    std::string_view type { "UNKNOWN" };
    switch (token.type) {
    case Lexer::TokenType::Colon:
      type = "Colon";
      break;
    case Lexer::TokenType::Comma:
      type = "Comma";
      break;
    case Lexer::TokenType::Dot:
      type = "Dot";
      break;
    case Lexer::TokenType::Dollar:
      type = "Dollar";
      break;
    case Lexer::TokenType::Minus:
      type = "Minus";
      break;
    case Lexer::TokenType::Equal:
      type = "Equal";
      break;
    case Lexer::TokenType::LeftParen:
      type = "LeftParen";
      break;
    case Lexer::TokenType::RightParen:
      type = "RightParen";
      break;
    case Lexer::TokenType::LeftSquare:
      type = "LeftSquare";
      break;
    case Lexer::TokenType::RightSquare:
      type = "RightSquare";
      break;
    case Lexer::TokenType::NewLine:
      type = "NewLine";
      break;
    case Lexer::TokenType::EndOfFile:
      type = "EndOfFile";
      break;
    case Lexer::TokenType::String:
      type = "String";
      break;
    case Lexer::TokenType::Number:
      type = "Number";
      break;
    case Lexer::TokenType::Identifier:
      type = "Identifier";
      break;
    }

    if (token.type == Lexer::TokenType::NewLine || token.type == Lexer::TokenType::EndOfFile) {
      std::println(out, "{}", type);
    } else {
      std::println(out, "{:<15}{}", type, token.value);
    }
    return out;
  }

  Lexer::Lexer(const AssemblerData& data)
    : m_assembler_data { data } {}

  Error Lexer::createError(ErrorType type, size_t line_index, size_t column_index) const {
    return Error { type,
                   m_assembler_data.input_file_path,
                   m_assembler_data.lines[line_index],
                   line_index + 1,
                   column_index + 1 };
  }

  size_t Lexer::runOnString(size_t line_index, size_t column_index) {
    const auto& line = m_assembler_data.lines[line_index];

    const auto string_end = line.find_first_of('"', column_index + 1);
    if (string_end == std::string::npos) {
      throw createError(ErrorType::Lexer_NonPairedStringQuote, line_index, column_index);
    }

    const auto substring = line.substr(column_index + 1, string_end - column_index - 1);
    m_tokens.emplace_back(TokenType::String, substring, line_index, column_index);

    return string_end;  // no -1 because quote char is skipped
  }

  size_t Lexer::runOnNumber(size_t line_index, size_t column_index) {
    const auto& line = m_assembler_data.lines[line_index];

    size_t number_begin = column_index;
    char base = 'd';

    if (
      line[column_index] == '0'
      && column_index + 1 < line.size()
      && isBaseSymbol(line[column_index + 1])
    )
    {
      number_begin = column_index + 2;
      base = static_cast<char>(std::tolower(static_cast<unsigned char>(line[column_index + 1])));
    }

    if (line.size() == number_begin) {
      throw createError(ErrorType::Lexer_MissingDigitsAfterBase, line_index, number_begin);
    }

    size_t number_end {};
    switch (base) {
    case 'd':
      number_end = line.find_first_not_of(DIGITS_DEC, number_begin);
      break;
    case 'x':
      number_end = line.find_first_not_of(DIGITS_HEX, number_begin);
      break;
    case 'o':
      number_end = line.find_first_not_of(DIGITS_OCT, number_begin);
      break;
    case 'b':
      number_end = line.find_first_not_of(DIGITS_BIN, number_begin);
      break;
    default:
      assert(!"[Lexer] Invalid number base");
    }

    if (base != 'd' && number_end <= number_begin) {
      throw createError(ErrorType::Lexer_MissingDigitsAfterBase, line_index, number_begin);
    }

    const auto number = line.substr(column_index, number_end - column_index);
    m_tokens.emplace_back(TokenType::Number, number, line_index, column_index);

    return number_end - 1;
  }

  size_t Lexer::runOnIdentifier(size_t line_index, size_t column_index) {
    const auto& line = m_assembler_data.lines[line_index];

    const auto begin = column_index;
    for (++column_index; column_index < line.size(); ++column_index) {
      if (!std::isalnum(line[column_index]) && line[column_index] != '_') {
        break;
      }
    }
    const auto identifier = line.substr(begin, column_index - begin);
    m_tokens.emplace_back(TokenType::Identifier, identifier, line_index, begin);

    return column_index - 1;
  }

  void Lexer::runOnLine(size_t line_index) {
    const auto& line = m_assembler_data.lines[line_index];

    for (size_t column_index = 0; column_index < line.size(); ++column_index) {
      if (line[column_index] == ':') {
        m_tokens
          .emplace_back(TokenType::Colon, line.substr(column_index, 1), line_index, column_index);
      } else if (line[column_index] == ',') {
        m_tokens
          .emplace_back(TokenType::Comma, line.substr(column_index, 1), line_index, column_index);
      } else if (line[column_index] == '.') {
        m_tokens
          .emplace_back(TokenType::Dot, line.substr(column_index, 1), line_index, column_index);
      } else if (line[column_index] == '$') {
        m_tokens
          .emplace_back(TokenType::Dollar, line.substr(column_index, 1), line_index, column_index);
      } else if (line[column_index] == '-') {
        m_tokens
          .emplace_back(TokenType::Minus, line.substr(column_index, 1), line_index, column_index);
      } else if (line[column_index] == '=') {
        m_tokens
          .emplace_back(TokenType::Equal, line.substr(column_index, 1), line_index, column_index);
      } else if (line[column_index] == '(') {
        m_tokens.emplace_back(
          TokenType::LeftParen,
          line.substr(column_index, 1),
          line_index,
          column_index
        );
      } else if (line[column_index] == ')') {
        m_tokens.emplace_back(
          TokenType::RightParen,
          line.substr(column_index, 1),
          line_index,
          column_index
        );
      } else if (line[column_index] == '[') {
        m_tokens.emplace_back(
          TokenType::LeftSquare,
          line.substr(column_index, 1),
          line_index,
          column_index
        );
      } else if (line[column_index] == ']') {
        m_tokens.emplace_back(
          TokenType::RightSquare,
          line.substr(column_index, 1),
          line_index,
          column_index
        );
      } else if (line[column_index] == '"') {
        column_index = runOnString(line_index, column_index);
      } else if (std::isdigit(line[column_index])) {
        column_index = runOnNumber(line_index, column_index);
      } else if (std::isalpha(line[column_index])) {
        column_index = runOnIdentifier(line_index, column_index);
      } else if (line[column_index] == '#') {
        break;
      } else if (std::isspace(line[column_index])) {
        continue;
      } else {
        throw createError(ErrorType::Lexer_UnexpectedSymbol, line_index, column_index);
      }
    }
  }

  const std::vector<Lexer::Token>& Lexer::run() {
    m_tokens.clear();

    size_t line_index = 0;
    for (; line_index < m_assembler_data.lines.size(); ++line_index) {
      runOnLine(line_index);
      m_tokens.emplace_back(
        TokenType::NewLine,
        "\n",
        line_index,
        m_assembler_data.lines[line_index].size() + 1
      );
    }

    m_tokens.emplace_back(TokenType::EndOfFile, "", line_index, 0);

    return m_tokens;
  }
}

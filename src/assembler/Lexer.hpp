#pragma once

#include "AssemblerData.hpp"
#include "AssemblerError.hpp"

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

namespace assembler {
  class Lexer {
  public:
    enum class TokenType {
      Colon,
      Comma,
      Dot,
      Dollar,
      
      Minus,
      Equal,
      
      LeftParen,
      RightParen,
      LeftSquare,
      RightSquare,
      
      NewLine,
      EndOfFile,
      
      String,
      Number,
      
      Identifier,
    };
    
    struct Token {
      TokenType type;
      std::string value;
      size_t line;
      size_t column;

      friend std::ostream& operator<<(std::ostream& out, const Token& token);
    };

  public:
    Lexer(const AssemblerData& data);
    
    const std::vector<Token>& run();

  private:
    Error createError(ErrorType type, size_t line_index = -1, size_t column_index = -1) const;
    
    void runOnLine(size_t line_index);

    size_t runOnString(size_t line_index, size_t column_index);
    size_t runOnNumber(size_t line_index, size_t column_index);
    size_t runOnIdentifier(size_t line_index, size_t column_index);

  private:
    const AssemblerData& m_assembler_data;

    std::vector<Token> m_tokens{};
  };
}

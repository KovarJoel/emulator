#pragma once

#include "AssemblerData.hpp"

#include <cstddef>
#include <string>
#include <ostream>

namespace assembler {
  enum class ErrorType {
    Assembler_InvalidArgumentCount,
    Assembler_InvalidFilePath,
    Assembler_ReadError,
    
    Lexer_UnexpectedSymbol,
    Lexer_NonPairedStringQuote,
    Lexer_MissingDigitsAfterBase,

    Parser_UnexpectedLineStart,
    Parser_ExpectedEndOfLine,

    Parser_ExpectedSegment,
    Parser_MissingSegment,
    Parser_MissingSegmentColon,
    Parser_MissingSegmentType,

    Parser_ExpectedFunctionDeclaration,
    Parser_MissingFunctionName,
    Parser_MissingFunctionColon,

    Parser_ExpectedBranchLabel,
    Parser_ExpectedBranchTarget,
    Parser_ExpectedJumpTarget,
    
    Parser_ExpectedVariableName,
    Parser_ExpectedEquals,
    Parser_ExpectedWidthSpecifier,
    Parser_InvalidWidthSpecifier,
    Parser_ExpectedInitializerValue,
    Parser_ExpectedEndOfInitializer,

    Parser_ExpectedMnemonic,
    Parser_UnknownMnemonic,
    
    Parser_NumericValueOutOfRange,
    Parser_ExpectedNumber,
    Parser_ExpectedNumberAfterMinus,

    Parser_MissingOperands,
    Parser_ExpectedOperand,
    Parser_ExpectedRegisterOperand,
    Parser_InvalidRegisterAddress,
    Parser_ExpectedAddressingMode,
    Parser_ExpectedOperandSeparator,
    Parser_ExpectedString,

    // Encoder_MissingBranch,
    // Encoder_DuplicateBranch,
  };

  struct Error {
    ErrorType type{};

    std::string file_path{};
    
    std::string line_content{};
    size_t line{};
    size_t column{};

    int argc{};
    int expected_argc{};
    std::string usage{};
  };

  std::ostream& operator<<(std::ostream& out, const Error& error);
}

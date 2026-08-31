#pragma once

#include <cstddef>
#include <ostream>
#include <string>

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

    Parser_ExpectedImmediate,
    Parser_NumericValueOutOfRange,
    Parser_ExpectedNumber,
    Parser_ExpectedNumberAfterMinus,
    Parser_NegativeUnsigned,
    Parser_ExpectedSizeOneCharImmediate,

    Parser_MissingOperands,
    Parser_ExpectedOperand,
    Parser_ExpectedRegisterOperand,
    Parser_InvalidRegisterAddress,
    Parser_ExpectedAddressingMode,
    Parser_ExpectedOperandSeparator,
    Parser_ExpectedString,
    Parser_EmptyString,

    IRGenerator_MissingMain,
    IRGenerator_UndefinedBranch,
    IRGenerator_ShaddowingBranch,
    IRGenerator_VoidBranch,
    IRGenerator_DuplicateBranch,
    IRGenerator_DuplicateFunction,
    IRGenerator_EmptyFunction,

    IRGenerator_UndefinedVariable,
    IRGenerator_InsufficientWidthForImmediate
  };

  struct Error {
    ErrorType type {};

    std::string file_path {};

    std::string line_content {};
    size_t line {};
    size_t column {};

    int argc {};
    int expected_argc {};
    std::string usage {};
  };

  std::ostream& operator<<(std::ostream& out, const Error& error);
}

#include "AssemblerError.hpp"

#include <print>

namespace assembler {
  std::ostream& operator<<(std::ostream& out, const Error& error) {
    std::print(out, "[ERROR] ");
    switch (error.type) {
    case ErrorType::Assembler_InvalidArgumentCount:
      std::println(out, "Assembler: Invalid argument count, expected {}, got {}", error.expected_argc, error.argc);
      std::println("  > Usage: {}", error.usage);
      return out;
    case ErrorType::Assembler_InvalidFilePath:
      std::println(out, "Assembler: Invalid file path ({})", error.file_path);
      return out;

    case ErrorType::Assembler_ReadError:
      std::println(out, "Lexer: Failed to read from file");
      break;

    case ErrorType::Lexer_UnexpectedSymbol:
      std::println(out, "Lexer: Read an unexpected symbol");
      break;
    case ErrorType::Lexer_NonPairedStringQuote:
      std::println(out, "Lexer: Detected non paired string quote (\")");
      break;
    case ErrorType::Lexer_MissingDigitsAfterBase:
      std::println(out, "Lexer: Missing digits after the base specifier");
      break;

    case ErrorType::Parser_UnexpectedLineStart:
      std::println(out, "Parser: Unexpected token at beginning of line");
      break;
    case ErrorType::Parser_ExpectedEndOfLine:
      std::println(out, "Parser: Expected end of line");
      break;
    case ErrorType::Parser_ExpectedSegment:
      std::println(out, "Parser: Expected segment declaration");
      break;
    case ErrorType::Parser_MissingSegment:
      std::println(out, "Parser: Missing preceding segment declaration");
      break;
    case ErrorType::Parser_MissingSegmentColon:
      std::println(out, "Parser: Missing colon after segment keyword");
      break;
    case ErrorType::Parser_MissingSegmentType:
      std::println(out, "Parser: Missing segment type (code/data)");
      break;
    case ErrorType::Parser_ExpectedFunctionDeclaration:
      std::println(out, "Parser: Expected function declaration");
      break;
    case ErrorType::Parser_MissingFunctionName:
      std::println(out, "Parser: Missing function name");
      break;
    case ErrorType::Parser_MissingFunctionColon:
      std::println(out, "Parser: Missing colon after function name");
      break;
    case ErrorType::Parser_ExpectedBranchLabel:
      std::println(out, "Parser: Expected branch label before colon");
      break;
    case ErrorType::Parser_ExpectedBranchTarget:
      std::println(out, "Parser: Expected branch target");
      break;
    case ErrorType::Parser_ExpectedJumpTarget:
      std::println(out, "Parser: Expected jump target");
      break;
    case ErrorType::Parser_ExpectedVariableName:
      std::println(out, "Parser: Expected a variable name");
      break;
    case ErrorType::Parser_ExpectedEquals:
      std::println(out, "Parser: Expected an equals sign after variable name");
      break;
    case ErrorType::Parser_ExpectedWidthSpecifier:
      std::println(out, "Parser: Expected width specifier after dot in variable assignment");
      break;
    case ErrorType::Parser_InvalidWidthSpecifier:
      std::println(out, "Parser: Invalid width specifier. Supported are B (byte), H (half word), W (word, default)");
      break;
    case ErrorType::Parser_ExpectedInitializerValue:
      std::println(out, "Parser: Expected initializer value");
      break;
    case ErrorType::Parser_ExpectedEndOfInitializer:
      std::println(out, "Parser: Expected end of variable initializer (newline or end of file)");
      break;
    case ErrorType::Parser_ExpectedMnemonic:
      std::println(out, "Parser: Expected mnemonic");
      break;
    case ErrorType::Parser_UnknownMnemonic:
      std::println(out, "Parser: Unknown mnemonic");
      break;
    case ErrorType::Parser_ExpectedImmediate:
      std::println(out, "Parser: Expected immediate value (number or variable address)");
      break;
    case ErrorType::Parser_NumericValueOutOfRange:
      std::println(out, "Parser: Numeric value is outside of the valid word range");
      break;
    case ErrorType::Parser_ExpectedNumber:
      std::println(out, "Parser: Expected a number");
      break;
    case ErrorType::Parser_ExpectedNumberAfterMinus:
      std::println(out, "Parser: Expected a number after a minus sign");
      break;
    case ErrorType::Parser_MissingOperands:
      std::println(out, "Parser: Missing operands for given instruction");
      break;
    case ErrorType::Parser_ExpectedOperand:
      std::println(out, "Parser: Expected operand for given instruction");
      break;
    case ErrorType::Parser_ExpectedRegisterOperand:
      std::println(out, "Parser: Expected register operand for given instruction");
      break;
    case ErrorType::Parser_InvalidRegisterAddress:
      std::println(out, "Parser: Invalid register address, valid range is [0, 16)");
      break;
    case ErrorType::Parser_ExpectedAddressingMode:
      std::println(out, "Parser: Expected addressing mode or associated symbol");
      break;
    case ErrorType::Parser_ExpectedOperandSeparator:
      std::println(out, "Parser: Expected separator between operands");
      break;
    case ErrorType::Parser_ExpectedString:
      std::println(out, "Parser: Expected string");
      break;
    case ErrorType::IRGenerator_MissingMain:
      std::println(out, "IRGenerator: Missing main function, there is no entry point");
      break;
    case ErrorType::IRGenerator_UndefinedBranch:
      std::println(out, "IRGenerator: Undefined branch");
      break;
    case ErrorType::IRGenerator_ShaddowingBranch:
      std::println(out, "IRGenerator: Branch shaddows function label");
      break;
    case ErrorType::IRGenerator_VoidBranch:
      std::println(out, "IRGenerator: Branch is pointing to nothing, missing instruction afterwards");
      break;
    case ErrorType::IRGenerator_DuplicateBranch:
      std::println(out, "IRGenerator: Branch with same label already exists elsewhere in function");
      break;
    case ErrorType::IRGenerator_DuplicateFunction:
      std::println(out, "IRGenerator: Function with same label already exists elsewhere");
      break;
    case ErrorType::IRGenerator_EmptyFunction:
      std::println(out, "IRGenerator: Empfy function, expected instructions");
      break;
    case ErrorType::IRGenerator_UndefinedVariable:
      std::println(out, "IRGenerator: Undefined variable name");
      break;
    }
    
    std::println(out, "  > In file {}:{}:{}", error.file_path, error.line, error.column);
    
    if (error.line == 0) return out;

    std::println(out, "{:5} | {}", error.line, error.line_content);

    if (error.column == 0) return out;
    
    std::print(out, "{:5} | ", ' ');
    for (size_t i = 1; i < error.column; ++i) {
      std::print(out, "-");
    }
    std::println(out, "^");

    return out;
  }
}

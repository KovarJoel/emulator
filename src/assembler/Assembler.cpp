#include "Assembler.hpp"

#include "AssemblerError.hpp"
#include "Encoder.hpp"
#include "IRGenerator.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

#include <fstream>
#include <ios>
#include <iostream>

namespace assembler {
  Assembler::Assembler(int argc, const char** argv) {
    constexpr int EXPECTED_ARGC = 3;
    if (argc != EXPECTED_ARGC) {
      throw Error { .type = ErrorType::Assembler_InvalidArgumentCount,
                    .argc = argc,
                    .expected_argc = EXPECTED_ARGC,
                    .usage = "assembler <input-file-path> <output-file-path>" };
    }

    m_data.input_file_path = argv[1];
    m_data.output_file_path = argv[2];
  }

  void Assembler::run() {
    readFile();

    Lexer lexer { m_data };
    const auto& lexer_tokens = lexer.run();

    Parser parser { m_data };
    const auto& parser_tokens = parser.run(lexer_tokens);

    IRGenerator ir_gen { m_data };
    const auto& ir = ir_gen.run(parser_tokens);

    Encoder encoder {};
    const auto& bytes = encoder.run(ir);

    writeFile(bytes);
  }

  void Assembler::readFile() {
    std::ifstream file { m_data.input_file_path };

    if (!file) {
      throw Error { ErrorType::Assembler_InvalidFilePath, m_data.input_file_path };
    }

    m_data.lines.clear();

    std::string line {};
    while (std::getline(file, line)) {
      m_data.lines.push_back(std::move(line));
    }

    if (!file.eof()) {
      if (m_data.lines.empty()) {
        throw Error { ErrorType::Assembler_ReadError, m_data.input_file_path };
      } else {
        throw Error { ErrorType::Assembler_ReadError,
                      m_data.input_file_path,
                      m_data.lines.back(),
                      m_data.lines.size(),
                      m_data.lines.back().size() };
      }
    }
  }

  void Assembler::writeFile(std::span<const std::byte> bytes) {
    std::ofstream file { m_data.output_file_path, std::ios_base::out | std::ios_base::binary };
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
  }
}

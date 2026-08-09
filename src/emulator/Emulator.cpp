#include "Emulator.hpp"

#include "core/Console.hpp"
#include "core/Exceptions.hpp"
#include "core/Instructions/Operations.hpp"
#include "core/ProcessorState.hpp"
#include "core/Instructions/InstructionData.hpp"

#include <format>
#include <iostream>
#include <print>
#include <sstream>

namespace emulator {
  bool Emulator::setArgs(int argc, const char** argv) {
    if (argc != 2) {
      std::println(std::cerr, "[ERROR] > Invalid number of arguments ({}), expected 2", argc);
      return false;
    }
    m_binary_path = argv[1];

    try {
      m_processor.loadProgram(m_binary_path);
    }
    catch (const core::exceptions::InvalidBinary& e) {
      std::println(std::cerr, "[ERROR] > Failed to load binary from {}", m_binary_path);
      return false;
    }
    return true;
  }

  static std::string rgbToAnsi(core::Console::Color color) {
    return std::format("\x1b[38;2;{};{};{}m", color.red, color.green, color.blue);
  }

  static std::string moveToPreviousLineBeginning(size_t count = 1) {
    return std::format("\x1b[{}F", count);
  }

  static void printConsole(const core::Console& console) {
    static bool firstDraw = true;

    std::stringstream stream{};
    
    if (!firstDraw) {
      stream << moveToPreviousLineBeginning(console.HEIGHT + 2);
    }
    firstDraw = false;

    stream << rgbToAnsi({0xFF, 0xFF, 0xFF});
    stream << '+';
    for (int32_t x = 0; x < console.WIDTH; ++x) {
      stream << '-';
    }
    stream << "+\n";

    for (int32_t y = 0; y < console.HEIGHT; ++y) {
      stream << rgbToAnsi({0xFF, 0xFF, 0xFF});
      stream << '|';

      for (int32_t x = 0; x < console.WIDTH; ++x) {
        auto& cell = console.cells[y * console.HEIGHT + x];

        stream << rgbToAnsi(cell.color);
        if (std::isprint(cell.symbol)) {
          stream << static_cast<char>(cell.symbol);
        }
        else {
          stream << ' ';
        }
      }
      
      stream << rgbToAnsi({0xFF, 0xFF, 0xFF});
      stream << "|\n";
    }

    stream << rgbToAnsi({0xFF, 0xFF, 0xFF});
    stream << '+';
    for (int32_t x = 0; x < console.WIDTH; ++x) {
      stream << '-';
    }
    stream << "+\n";

    std::cout << stream.str();
  }

  static void callback(core::ProcessorState& state, const core::instructions::InstructionData& instruction) {
    static core::instructions::InstructionData lastInstruction{};

    using enum core::instructions::Opcode;
    if (lastInstruction.getOpcode() == ST) {
      printConsole(state.memory.getConsole());
    }

    lastInstruction = instruction;
  }

  void Emulator::run() {
    if (m_binary_path.empty()) return;

    printConsole(m_processor.getState().memory.getConsole());
    m_processor.run(callback);
    printConsole(m_processor.getState().memory.getConsole());
  }
}

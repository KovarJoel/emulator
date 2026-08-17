#include "Emulator.hpp"

#include "core/Console.hpp"
#include "core/Exceptions.hpp"
#include "core/Instructions/Operations.hpp"
#include "core/ProcessorState.hpp"
#include "core/Instructions/InstructionData.hpp"

#include <iostream>
#include <print>

#include <ftxui/ftxui.hpp>

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

  static void printConsole(const core::Console& console, bool reset = true) {
    static auto screen = ftxui::Screen::Create(
      ftxui::Dimension::Fixed(core::Console::WIDTH + 2),
      ftxui::Dimension::Fixed(core::Console::HEIGHT + 2)
    );

    for (int32_t y = 0; y < core::Console::HEIGHT; ++y) {
      for (int32_t x = 0; x < core::Console::WIDTH; ++x) {
        auto& screen_cell = screen.CellAt(x + 1, y + 1);
        const auto& console_cell = console.cells[y * core::Console::WIDTH + x]; 

        screen_cell.foreground_color = ftxui::Color::RGB(console_cell.color.red, console_cell.color.green, console_cell.color.blue);
        screen_cell.character = std::isprint(console_cell.symbol) ? console_cell.symbol : ' ';
      }
    }

    ftxui::Render(screen, ftxui::border(ftxui::emptyElement()));

    std::cout << screen.ToString();
    if (reset) {
      std::cout << screen.ResetPosition();
    }
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
    printConsole(m_processor.getState().memory.getConsole(), false);
  }
}

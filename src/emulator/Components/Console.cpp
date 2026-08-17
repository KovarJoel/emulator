#include "Console.hpp"
#include <ftxui/screen/color.hpp>

#include <mutex>

namespace emulator {
  Console::Console(core::ProcessorState& state)
    : m_state{ state } {
    }

  ftxui::Element Console::OnRender() {
    constexpr size_t X_SCALE = 2;
    constexpr size_t Y_SCALE = 4;

    ftxui::Canvas canvas{ core::Console::WIDTH * X_SCALE, core::Console::HEIGHT * Y_SCALE };

    std::lock_guard<std::mutex> lock{ m_state.memory.getMutex() };
    for (int32_t y = 0; y < core::Console::HEIGHT; ++y) {
      for (int32_t x = 0; x < core::Console::WIDTH; ++x) {
        const auto& console_cell = m_state.memory.getConsole().cells[y * core::Console::WIDTH + x]; 

        ftxui::Cell cell{};
        cell.foreground_color = ftxui::Color::RGB(console_cell.color.red, console_cell.color.green, console_cell.color.blue);
        cell.character = std::isprint(console_cell.symbol) ? console_cell.symbol : ' ';

        canvas.DrawCell(x * X_SCALE, y * Y_SCALE, cell);
      }
    }

    return ftxui::canvas(canvas) | ftxui::border;
  }

  bool Console::OnEvent(ftxui::Event event) {
    if (!event.is_character()) return false;
    if (event.character().size() != 1) return false;
    if (!std::isprint(event.character()[0])) return false;

    std::lock_guard<std::mutex> lock{ m_state.memory.getMutex() };
    m_state.memory.getKeyEvents().enqueueEvent(event.character()[0]);

    return true;
  }
}

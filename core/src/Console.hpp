#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace emulator::core {
  class Console {
  public:
    constexpr static size_t HEIGHT{ 42 };
    constexpr static size_t WIDTH{ 128 };

    struct Position {
      int32_t x{};
      int32_t y{};
    };

    struct Color {
      uint8_t red{ 0xFF };
      uint8_t green{ 0xFF };
      uint8_t blue{ 0xFF };
    };

    struct Cell {
      Color color{};
      uint8_t symbol{};
    };

    struct Bookkeeping {
      Position next_write{};
    };

  public:
    std::array<Cell, HEIGHT * WIDTH> cells{};
    Bookkeeping bookkeeping{};
  };
}

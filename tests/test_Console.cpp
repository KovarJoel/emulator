#include <catch2/catch_test_macros.hpp>

#include "Console.hpp"

#include <type_traits>

using namespace emulator::core;

TEST_CASE("Sizes", "[Memory::Mappings::Console]") {
  STATIC_CHECK(Console::WIDTH == 128);
  STATIC_CHECK(Console::HEIGHT == 42);

  STATIC_CHECK(sizeof(Console::Color) == 3);
  STATIC_CHECK(sizeof(Console::Cell) == 4);
  STATIC_CHECK(sizeof(Console::cells) == 0x5400);
  STATIC_CHECK(sizeof(Console::cells) == 128 * 42 * 4);
  STATIC_CHECK(sizeof(Console::bookkeeping) + sizeof(Console::cells) <= 0x6000);
  STATIC_CHECK(sizeof(Console::cells) + sizeof(Console::bookkeeping) == sizeof(Console));
}

TEST_CASE("Type Traits", "[Memory::Mappings::Console]") {
  STATIC_CHECK(std::is_trivially_copyable_v<Console>);
  STATIC_CHECK(std::is_implicit_lifetime_v<Console>);
}

TEST_CASE("Memory Order", "[Memory::Mappings::Console]") {
  STATIC_CHECK(offsetof(Console, cells) == 0);
  STATIC_CHECK(offsetof(Console, bookkeeping) == 0x5400);
  STATIC_CHECK(offsetof(Console::Cell, color) == 0);
  STATIC_CHECK(offsetof(Console::Cell, symbol) == 3);
  STATIC_CHECK(offsetof(Console::Color, red) == 0);
  STATIC_CHECK(offsetof(Console::Color, green) == 1);
  STATIC_CHECK(offsetof(Console::Color, blue) == 2);
  STATIC_CHECK(offsetof(Console::Bookkeeping, next_write) == 0);
  STATIC_CHECK(offsetof(Console::Position, x) == 0);
  STATIC_CHECK(offsetof(Console::Position, y) == 4);
}

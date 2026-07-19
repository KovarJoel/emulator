#include <catch2/catch_test_macros.hpp>

#include "Memory.hpp"
#include "Exceptions.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <span>
#include <type_traits>
#include <utility>

using namespace emulator::core;

namespace {
  constexpr std::array<char, 3 * Memory::PAGE_SIZE> valid_binary{
    'e', 'm', 'u', 'l', 'a', 't', 'o', 'r',
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x20, 0x01, 0x00,
    0x00, 0x30, 0x01, 0x00,
    0x00, 0x1a, 0x01, 0x00
  };
  
  const std::span<const std::byte> valid_binary_span{
    reinterpret_cast<const std::byte*>(valid_binary.data()), valid_binary.size()
  };
}

TEST_CASE("Sizes", "[Memory]") {
  STATIC_CHECK(Memory::PAGE_SIZE == 0x1000);
  STATIC_CHECK(Memory::RAM_SIZE == 1 << 24);

  STATIC_CHECK(sizeof(Memory::Header) <= Memory::PAGE_SIZE);
}

TEST_CASE("Alignment", "[Memory]") {
  STATIC_CHECK(Memory::OFFSET_CONSOLE % std::to_underlying(Memory::MAX_ALIGNMENT) == 0);
  STATIC_CHECK(Memory::OFFSET_HEADER % std::to_underlying(Memory::MAX_ALIGNMENT) == 0);

  Memory memory;
  CHECK(reinterpret_cast<uintptr_t>(&memory.getHeader()) % alignof(Memory::Header) == 0);
  CHECK(reinterpret_cast<uintptr_t>(&memory.getConsole()) % alignof(Console) == 0);
}

TEST_CASE("Endianness", "[Memory]") {
  STATIC_CHECK(std::endian::native == std::endian::little);
}

TEST_CASE("Type Traits", "[Memory]") {
  STATIC_CHECK(std::is_trivially_copyable_v<Memory::Header>);
}

TEST_CASE("Invalid Read Accesses", "[Memory]") {
  using namespace exceptions;
  
  Memory memory;
  CHECK_NOTHROW(memory.loadProgram(valid_binary_span));
  
  CHECK_THROWS_AS(memory.get<uint8_t>(0), InvalidReadAccess);
  CHECK_THROWS_AS(memory.get<uint8_t>(0x0fff), InvalidReadAccess);
  CHECK_NOTHROW(memory.get<uint8_t>(0x1000));
  
  CHECK_NOTHROW(memory.get<uint8_t>(Memory::OFFSET_HEADER));
  CHECK_NOTHROW(memory.get<uint8_t>(memory.getHeader().code_begin));
  CHECK_NOTHROW(memory.get<uint8_t>(memory.getHeader().data_begin));
  CHECK_NOTHROW(memory.get<uint8_t>(memory.getHeader().ram_begin));

  CHECK_NOTHROW(memory.get<uint8_t>(Memory::RAM_SIZE - 1));
  CHECK_THROWS_AS(memory.get<uint8_t>(Memory::RAM_SIZE), InvalidReadAccess);
  
  CHECK_NOTHROW(memory.get<int32_t>(Memory::RAM_SIZE - 4));
  CHECK_THROWS_AS(memory.get<int32_t>(Memory::RAM_SIZE - 3), InvalidReadAccess);
}

TEST_CASE("Invalid Write Accesses", "[Memory]") {
  using namespace exceptions;
  
  Memory memory;
  CHECK_NOTHROW(memory.loadProgram(valid_binary_span));
  
  CHECK_THROWS_AS(memory.set<uint8_t>(0, 0x00), InvalidWriteAccess);
  CHECK_THROWS_AS(memory.set<uint8_t>(0x0fff, 0x00), InvalidWriteAccess);
  CHECK_NOTHROW(memory.set<uint8_t>(0x1000, 0x00));
  
  CHECK_THROWS_AS(memory.set<uint8_t>(Memory::OFFSET_HEADER, 0x00), InvalidWriteAccess);
  CHECK_THROWS_AS(memory.set<uint8_t>(memory.getHeader().code_begin, 0x00), InvalidWriteAccess);
  CHECK_NOTHROW(memory.set<uint8_t>(memory.getHeader().data_begin, 0x00));
  CHECK_NOTHROW(memory.set<uint8_t>(memory.getHeader().ram_begin, 0x00));

  CHECK_NOTHROW(memory.set<uint8_t>(Memory::RAM_SIZE - 1, 0x00));
  CHECK_THROWS_AS(memory.set<uint8_t>(Memory::RAM_SIZE, 0x00), InvalidWriteAccess);
  
  CHECK_NOTHROW(memory.set<int32_t>(Memory::RAM_SIZE - 4, 0x00));
  CHECK_THROWS_AS(memory.set<int32_t>(Memory::RAM_SIZE - 3, 0x00), InvalidWriteAccess);
}

TEST_CASE("Memory Order", "[Memory]") {
  Memory memory;
  CHECK_NOTHROW(memory.loadProgram(valid_binary_span));

  
  std::array<uint8_t, Memory::PAGE_SIZE> buffer{};
  auto fillBuffer = [&](size_t offset) {
      for (size_t i = 0; i < Memory::PAGE_SIZE; ++i) {
        buffer[i] = memory.get<uint8_t>(offset + i);
      }
  };
  
  fillBuffer(Memory::OFFSET_CONSOLE);
  CHECK(std::memcmp(&memory.getConsole(), buffer.data(), sizeof(buffer)) == 0);
  memory.getConsole().cells[1 * Console::WIDTH + 23] = {.color{0xAB, 0xCD, 0xDE}, .symbol='?'};
  CHECK(std::memcmp(&memory.getConsole(), buffer.data(), sizeof(buffer)) != 0);
  fillBuffer(Memory::OFFSET_CONSOLE);
  CHECK(std::memcmp(&memory.getConsole(), buffer.data(), sizeof(buffer)) == 0);

  constexpr size_t console_bookkeeping_offset = 0x6400; 
  fillBuffer(console_bookkeeping_offset);
  CHECK(std::memcmp(&memory.getConsole().bookkeeping, buffer.data(), sizeof(buffer)) == 0);
  memory.getConsole().bookkeeping.next_write = {.x = 12, .y = 32};
  CHECK(std::memcmp(&memory.getConsole().bookkeeping, buffer.data(), sizeof(buffer)) != 0);
  fillBuffer(console_bookkeeping_offset);
  CHECK(std::memcmp(&memory.getConsole().bookkeeping, buffer.data(), sizeof(buffer)) == 0);

  fillBuffer(Memory::OFFSET_HEADER);
  CHECK(std::memcmp(&memory.getHeader(), buffer.data(), sizeof(buffer)) == 0);
  memory.getHeader().entry_point = 0xdeadbeef;
  memory.getHeader().ram_begin += 0x1000;
  CHECK(std::memcmp(&memory.getHeader(), buffer.data(), sizeof(buffer)) != 0);
  fillBuffer(Memory::OFFSET_HEADER);
  CHECK(std::memcmp(&memory.getHeader(), buffer.data(), sizeof(buffer)) == 0);
}

TEST_CASE("Load Program", "[Memory]") {
  const auto path = std::filesystem::temp_directory_path() / "emulator_test.bin";

  Memory memory;
  {
    CHECK_NOTHROW(memory.loadProgram(valid_binary_span));

    std::ofstream file(path, std::ios::binary);
    file.write(valid_binary.data(), valid_binary.size());
  }
  CHECK_NOTHROW(memory.loadProgram(path));

  std::filesystem::remove(path);

  auto toSpan = [](auto array) {
    return std::span(
      reinterpret_cast<const std::byte*>(array.data()),
      array.size() * sizeof(array[0])
    );
  };
  
  using namespace exceptions;
  
  auto invalid_binary = valid_binary;
  invalid_binary[0] = '?';
  CHECK_THROWS_AS(memory.loadProgram(toSpan(invalid_binary)), InvalidBinary);

  invalid_binary = valid_binary;
  invalid_binary[19] = 0xff;  // data_begin > ram_begin
  CHECK_THROWS_AS(memory.loadProgram(toSpan(invalid_binary)), InvalidBinary);

  invalid_binary = valid_binary;
  invalid_binary[20] = 0x12; // ram_begin not page aligned
  CHECK_THROWS_AS(memory.loadProgram(toSpan(invalid_binary)), InvalidBinary);
}

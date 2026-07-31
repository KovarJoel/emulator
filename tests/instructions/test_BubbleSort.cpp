#include <catch2/catch_test_macros.hpp>

#include "Instructions/RegisterOrImmediate.hpp"
#include "Processor.hpp"
#include "Instructions/InstructionData.hpp"

#include <array>
#include <limits>
#include <vector>
#include <span>
#include <cstddef>
#include <algorithm>

using namespace core;
using namespace core::instructions;

namespace {
  constexpr uint32_t BASE_ADDRESS = 0x20000;
  constexpr uint32_t VALUE_COUNT = 10;
  constexpr std::array<int32_t, VALUE_COUNT> VALUES{
    -1, 10, 0, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max(), 23, -523532, 10, 53252, 2
  };

  constexpr uint32_t ENTRY_POINT = Memory::OFFSET_CODE;
  constexpr uint32_t LABEL_OUTER = ENTRY_POINT + 2 * (3 + 3);
  constexpr uint32_t LABEL_INNER = LABEL_OUTER + 3;
  constexpr uint32_t LABEL_NO_SWAP = LABEL_INNER + 2 * 3 + 1 * 4 + 4 * (3 + 3);

  using enum Opcode;
  using enum InstructionData::Width;
  using enum RegisterOrImmediate::SourceMode;
  const auto INSTRUCTION_DATA = std::to_array<InstructionData>({
    { ADD,  Word, 1, { RegisterOrImmediate{ Immediate, BASE_ADDRESS } }},       // r1 = BASE_ADDRESS
    { ADD,  Word, 2, { RegisterOrImmediate{ Immediate, VALUE_COUNT - 1 } }},    // r2 = VALUE_COUNT - 1

    // outer loop
    { ADD,  Word, 3, {} },                                                      // r3 = 0

    // inner loop
    { ADD,  Word, 4, { RegisterOrImmediate{ Register, 1 },  { Register, 3 }}},  // r4 = r1 + r3
    { LD,   Word, 5, { RegisterOrImmediate{ Immediate, 0 }, { Register, 4 }}},  // r5 = Mem[r4]
    { LD,   Word, 6, { RegisterOrImmediate{ Immediate, 4 }, { Register, 4 }}},  // r6 = Mem[r4+4]
    { SUB,  Word, 0, { RegisterOrImmediate{ Register, 5 },  { Register, 6 }}},  // if r5 <= r6
    { BLE,  Word, 0, { RegisterOrImmediate{ Immediate, LABEL_NO_SWAP }}},       //    goto no swap
    { ST,   Word, 6, { RegisterOrImmediate{ Immediate, 0 }, { Register, 4 }}},  // Mem[r4] = r6
    { ST,   Word, 5, { RegisterOrImmediate{ Immediate, 4 }, { Register, 4 }}},  // Mem[r4+4] = r5

    // no swap
    { ADD,  Word, 3, { RegisterOrImmediate{ Register, 3 },  { Immediate, 4 }}}, // r3 += 4
    { ADD,  Word, 7, { RegisterOrImmediate{ Register, 2 }}},                    // r7 = r2
    { SLL,  Word, 7, { RegisterOrImmediate{ Register, 7 },  { Immediate, 2 }}}, // r7 *= 4
    { SUB,  Word, 0, { RegisterOrImmediate{ Register, 3 },  { Register, 7 }}},  // if r3 < r7
    { BLT,  Word, 0, { RegisterOrImmediate{ Immediate, LABEL_INNER }}},         //    goto inner
    { SUB,  Word, 2, { RegisterOrImmediate{ Register, 2 },  { Immediate, 1 }}}, // --r2
    { SUB,  Word, 0, { RegisterOrImmediate{ Register, 2 }}},                    // if r2 > 0
    { BGT,  Word, 0, { RegisterOrImmediate{ Immediate, LABEL_OUTER }}},         //    goto outer

    { HALT, Word, 0, {}}
  });

  auto createBinary() {
    std::vector<std::byte> instruction_data{};
    for (const auto& instruction : INSTRUCTION_DATA) {
      instruction_data.append_range(instruction.encode());
    }

    std::vector<std::byte> data{};

    Memory::Header header{};
    header.entry_point = header.code_begin;
    header.data_begin = BASE_ADDRESS;
    header.ram_begin = BASE_ADDRESS + sizeof VALUES + Memory::PAGE_SIZE;
    header.ram_begin = header.ram_begin + Memory::PAGE_SIZE - header.ram_begin % Memory::PAGE_SIZE;

    data.resize(header.code_begin - Memory::OFFSET_HEADER);
    memcpy(data.data(), &header, sizeof header);

    data.append_range(std::move(instruction_data));

    data.resize(header.data_begin - Memory::OFFSET_HEADER);
    for (const auto& value : VALUES) {
      data.append_range(std::bit_cast<std::array<std::byte, 4>>(value));
    }

    return data;
  }
}

TEST_CASE("Encoding and decoding bubble sort", "[Instructions]") {
  for (const auto& inst : INSTRUCTION_DATA) {
    auto copy = inst;
    copy.decode(copy.encode());
    CHECK(copy == inst);
  }
}

TEST_CASE("bubble sort", "[Instructions]") {
  Processor processor{};
  processor.loadProgram(createBinary());

  std::array<int32_t, VALUE_COUNT> values{};
  for (size_t i = 0; i < VALUE_COUNT; ++i) {
    values[i] = processor.getState().memory.get<int32_t>(BASE_ADDRESS + i * sizeof(values[i]));
  }
  CHECK(values == VALUES);

  processor.run();

  for (size_t i = 0; i < VALUE_COUNT; ++i) {
    values[i] = processor.getState().memory.get<int32_t>(BASE_ADDRESS + i * sizeof(values[i]));
  }
  auto sorted = VALUES;
  std::sort(sorted.begin(), sorted.end());
  CHECK(values == sorted);
}

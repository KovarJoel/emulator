#include <catch2/catch_test_macros.hpp>

#include "core/Instructions/Instructions.hpp"
#include "core/Processor.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <span>
#include <vector>

using namespace core;
using namespace core::instructions;

namespace {
  constexpr int32_t BASE_ADDRESS = 0x2'0000;
  constexpr int32_t VALUE_COUNT = 10;
  constexpr std::array<int32_t, VALUE_COUNT> VALUES { -1,
                                                      10,
                                                      0,
                                                      std::numeric_limits<int32_t>::min(),
                                                      std::numeric_limits<int32_t>::max(),
                                                      23,
                                                      -523'532,
                                                      10,
                                                      53'252,
                                                      2 };

  constexpr uint32_t ENTRY_POINT = Memory::OFFSET_CODE;
  constexpr int32_t LABEL_OUTER = ENTRY_POINT + 4 * InstructionData::ENCODING_SIZE;
  constexpr int32_t LABEL_INNER = LABEL_OUTER + 1 * InstructionData::ENCODING_SIZE;
  constexpr int32_t LABEL_NO_SWAP = LABEL_INNER + 7 * InstructionData::ENCODING_SIZE;

  using enum Opcode;
  const auto INSTRUCTION_DATA = std::to_array<InstructionData>({
    InstructionData::ParsedData { ITypeData { 1, 0, static_cast<int16_t>(BASE_ADDRESS >> 16) }, ADDI        }, // r1 = BASE_ADDRESS >> 16
    InstructionData::ParsedData { ITypeData { 1, 1, 16 }, SLLI                                              }, // r1 = (BASE_ADDRESS >> 16) << 16
    InstructionData::ParsedData { ITypeData { 1, 1, static_cast<int16_t>(BASE_ADDRESS) }, ORI               }, // r1 = BASE_ADDRESS
    InstructionData::ParsedData { ITypeData { 2, 0, VALUE_COUNT - 1 }, ADDI                                 }, // r2 = VALUE_COUNT - 1

    // outer loop
    InstructionData::ParsedData { RTypeData { 3, 0, 0 }, ADD                                                }, // r3 = 0

    // inner loop
    InstructionData::ParsedData { RTypeData { 4, 1, 3 }, ADD                                                }, // r4 = r1 + r3
    InstructionData::ParsedData { ITypeData { 5, 4, 0 }, LDI                                                }, // r5 = Mem[r4]
    InstructionData::ParsedData { ITypeData { 6, 4, 4 }, LDI                                                }, // r6 = Mem[r4 + 4]
    InstructionData::ParsedData { RTypeData { 0, 5, 6 }, SUB                                                }, // if r5 <= r6
    InstructionData::ParsedData { BTypeData { ConditionType::BranchIfLessEqual, LABEL_NO_SWAP }, B          }, //   goto no swap
    InstructionData::ParsedData { ITypeData { 6, 4, 0 }, STI                                                }, // Mem[r4] = r6
    InstructionData::ParsedData { ITypeData { 5, 4, 4 }, STI                                                }, // Mem[r4+4] = r5

    // no swap
    InstructionData::ParsedData { ITypeData { 3, 3, 4 }, ADDI                                               }, // r3 += 4
    InstructionData::ParsedData { RTypeData { 7, 0, 2 }, ADD                                                }, // r7 = r2
    InstructionData::ParsedData { ITypeData { 7, 7, 2 }, SLLI                                               }, // r7 *= 4
    InstructionData::ParsedData { RTypeData { 0, 3, 7 }, SUB                                                }, // if r3 < r7
    InstructionData::ParsedData { BTypeData { ConditionType::BranchIfLess, LABEL_INNER }, B                 }, //   goto inner
    InstructionData::ParsedData { ITypeData { 2, 2, 1 }, SUBI                                               }, // --r2
    InstructionData::ParsedData { RTypeData { 0, 2, 0 }, SUB                                                }, // if r2 > 0
    InstructionData::ParsedData { BTypeData { ConditionType::BranchIfGreater, LABEL_OUTER }, B              }, //   goto outer

    InstructionData::ParsedData { RTypeData { 0, 0, 0 }, HALT                                               }
  });

  auto createBinary() {
    std::vector<std::byte> instruction_data {};
    for (const auto& instruction : INSTRUCTION_DATA) {
      const auto raw = instruction.getRaw();
      for (const auto b : raw) {
        instruction_data.push_back(static_cast<std::byte>(b));
      }
    }

    std::vector<std::byte> data {};

    Memory::Header header {};
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
    copy = copy.getRaw();
    copy.encode(copy.decode());
    CHECK(copy.getRaw() == inst.getRaw());
  }
}

TEST_CASE("bubble sort", "[Instructions]") {
  Processor processor {};
  processor.loadProgram(createBinary());

  std::array<int32_t, VALUE_COUNT> values {};
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

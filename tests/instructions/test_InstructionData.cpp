#include <catch2/catch_test_macros.hpp>

#include "Instructions/InstructionData.hpp"
#include "Instructions/Operations.hpp"
#include "Instructions/RegisterOrImmediate.hpp"

using namespace core;
using namespace core::instructions;

TEST_CASE("Encoding register only", "[Instructions::InstructionData]") {
  using enum Opcode;
  using enum InstructionData::Width;
  using enum RegisterOrImmediate::SourceMode;

  InstructionData data{ ADD,  Word, 1, { RegisterOrImmediate{ Register, 2}, { Register, 3 }}};
  const auto encoding = data.encode();
  CHECK(encoding.size() == 3);
  CHECK(static_cast<Opcode>(encoding[0]) == ADD);
  
  const uint8_t remaining_1 = static_cast<uint8_t>(encoding[1]);
  CHECK(remaining_1 == 0b10'0001'00);
  
  const uint8_t remaining_2 = static_cast<uint8_t>(encoding[2]);
  CHECK(remaining_2 == 0b10'0'0011'0);
}

TEST_CASE("Encoding load with displacement", "[Instructions::InstructionData]") {
  using enum Opcode;
  using enum InstructionData::Width;
  using enum RegisterOrImmediate::SourceMode;

  InstructionData data{ LD,  HalfWord, 7, { RegisterOrImmediate{ Immediate, 12345 }, { Register, 3 }}};
  const auto encoding = data.encode();
  CHECK(encoding.size() == 6);
  CHECK(static_cast<Opcode>(encoding[0]) == LD);
  
  const uint8_t remaining_1 = static_cast<uint8_t>(encoding[1]);
  CHECK(remaining_1 == 0b01'0111'00);
  
  const uint8_t remaining_2 = static_cast<uint8_t>(encoding[2]);
  CHECK(remaining_2 == 0b00'1'0011'0);

  uint32_t immediate = 0;
  memcpy(&immediate, &encoding[3], 3);
  CHECK(immediate == 12345);
}

TEST_CASE("Encoding and decoding", "[Instructions::InstructionData]") {
  using enum Opcode;
  using enum InstructionData::Width;
  using enum RegisterOrImmediate::SourceMode;
  
  const InstructionData add{ ADD,  Word, 1, { RegisterOrImmediate{ Register, 2}, { Register, 3 }}};
  const InstructionData load{ LD,  HalfWord, 7, { RegisterOrImmediate{ Immediate, 12345 }, { Register, 3 }}};
  
  InstructionData copy = add;
  copy.decode(copy.encode());
  CHECK(copy == add);

  copy = load;
  copy.decode(copy.encode());
  CHECK(copy == load);
}

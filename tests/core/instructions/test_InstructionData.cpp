#include <catch2/catch_test_macros.hpp>

#include "core/Instructions/Instructions.hpp"

using namespace core;
using namespace core::instructions;

TEST_CASE("Encoding register only", "[Instructions::InstructionData]") {
  using enum Opcode;

  InstructionData data { InstructionData::ParsedData { RTypeData { 1, 2, 3 }, ADD }};
  const auto encoding = data.getRaw();

  CHECK(encoding.size() == InstructionData::ENCODING_SIZE);
  CHECK(static_cast<Opcode>(encoding[0]) == ADD);

  const uint8_t remaining_1 = static_cast<uint8_t>(encoding[1]);
  CHECK(remaining_1 == 0b0001'0010);

  const uint8_t remaining_2 = static_cast<uint8_t>(encoding[2]);
  CHECK(remaining_2 == 0b0011'0000);

  const uint8_t remaining_3 = static_cast<uint8_t>(encoding[3]);
  CHECK(remaining_3 == 0);
}

TEST_CASE("Encoding load with displacement", "[Instructions::InstructionData]") {
  using enum Opcode;

  InstructionData data { InstructionData::ParsedData { ITypeData { 7, 3, 12'345 }, LDHI }};

  const auto encoding = data.getRaw();
  CHECK(encoding.size() == InstructionData::ENCODING_SIZE);
  CHECK(static_cast<Opcode>(encoding[0]) == LDHI);

  const uint8_t remaining_1 = static_cast<uint8_t>(encoding[1]);
  CHECK(remaining_1 == 0b0111'0011);

  int16_t immediate = 0;
  memcpy(&immediate, &encoding[2], sizeof immediate);
  CHECK(immediate == 12'345);
}

TEST_CASE("Encoding and decoding", "[Instructions::InstructionData]") {
  using enum Opcode;

  InstructionData::ParsedData add { RTypeData { 1, 2, 3 }, ADD };
  InstructionData::ParsedData load { ITypeData { 7, 3, 12'345 }, LDHI };

  InstructionData copy = add;
  copy = copy.getRaw();

  CHECK(copy.decode().opcode == add.opcode);
  CHECK(std::get<RTypeData>(copy.decode().data).dest_reg_addr == std::get<RTypeData>(add.data).dest_reg_addr);
  CHECK(std::get<RTypeData>(copy.decode().data).src1_reg_addr == std::get<RTypeData>(add.data).src1_reg_addr);
  CHECK(std::get<RTypeData>(copy.decode().data).src2_reg_addr == std::get<RTypeData>(add.data).src2_reg_addr);

  copy = load;
  copy = copy.getRaw();

  CHECK(copy.decode().opcode == load.opcode);
  CHECK(std::get<ITypeData>(copy.decode().data).dest_reg_addr == std::get<ITypeData>(load.data).dest_reg_addr);
  CHECK(std::get<ITypeData>(copy.decode().data).src1_reg_addr == std::get<ITypeData>(load.data).src1_reg_addr);
  CHECK(std::get<ITypeData>(copy.decode().data).immediate_value == std::get<ITypeData>(load.data).immediate_value);
}

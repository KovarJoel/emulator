#include <catch2/catch_test_macros.hpp>

#include "core/Instructions/Instructions.hpp"
#include "core/ProcessorState.hpp"
#include "core/Register.hpp"
#include "core/Types.hpp"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <random>

using namespace core;
using namespace core::instructions;

namespace {
  ProcessorState state {};
  std::mt19937 random_engine {};
  std::uniform_int_distribution int_distribution { INT_MIN / 4, INT_MAX / 4 };
  std::uniform_int_distribution<uint32_t> register_distribution { 1, 10 };
  std::uniform_int_distribution bool_distribution { 0, 1 };

  template <EmulatorType T>
  InstructionData::ParsedData generateAdd(uint32_t dest_addr, T value1, T value2) {
    const auto reg_addr1 = register_distribution(random_engine);
    state.registers[reg_addr1].set(value1);

    if (value2 > INT16_MAX  || std::cmp_less(value2, INT16_MIN) || bool_distribution(random_engine)) {
      uint32_t reg_addr2 = 0;
      do {
        reg_addr2 = register_distribution(random_engine);
      } while (reg_addr1 == reg_addr2);
      state.registers[reg_addr2].set(value2);

      return InstructionData::ParsedData {
        .data = RTypeData { dest_addr, reg_addr1, reg_addr2 },
        .opcode = Opcode::ADD
      };
    } else {
      return InstructionData::ParsedData {
        .data = ITypeData { dest_addr, reg_addr1, static_cast<std::int16_t>(value2) },
        .opcode = Opcode::ADDI
      };
    }
  }
}

TEST_CASE("Random additions without overflow", "[Instructions::Operations::Add]") {
  for (uint32_t i = 0; i < 20; ++i) {
    const uint32_t dest_addr = register_distribution(random_engine);
    const int value1 = int_distribution(random_engine);
    const int value2 = int_distribution(random_engine);

    const int sum = value1 + value2;

    const InstructionData::ParsedData inst = generateAdd(dest_addr, value1, value2);
    execute(inst, state);

    CHECK(state.registers[dest_addr].get<int32_t>() == sum);

    CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Zero) == (sum == 0));
    CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign) == (sum < 0));

    const int min = std::min(value1, value2);
    const int max = std::max(value1, value2);
    CHECK(
      state.registers.getFLAGS().getBit(Register::FlagIndex::Carry)
      == ((value1 < 0 && value2 < 0) || (min < 0 && max > 0 && -max <= min))
    );
    CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow) == false);
  }
}

TEST_CASE("Overflow", "[Instructions::Operations::Add]") {
  InstructionData::ParsedData inst = generateAdd(1, INT32_MAX, 1);
  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == 0x8000'0000);
  CHECK(state.registers[1].get<int32_t>() == INT32_MIN);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));

  inst = generateAdd(1, -2, -INT32_MAX);
  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == 0x7FFF'FFFF);
  CHECK(state.registers[1].get<int32_t>() == INT_MAX);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));
}

TEST_CASE("Carry", "[Instructions::Operations::Add]") {
  InstructionData::ParsedData inst = generateAdd(1, 0xFFFF'FFFF, 1u);
  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == 0);
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));

  inst = generateAdd(1, 0xFFFF'FFFF, 0xFFFF'FFFF);
  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == 0xFFFF'FFFE);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));

  inst = generateAdd(1, 0xFFFF'FFFF, 0u);
  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == 0xFFFF'FFFF);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));
}

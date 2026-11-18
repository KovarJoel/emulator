#include <catch2/catch_test_macros.hpp>

#include "core/Instructions/Instructions.hpp"
#include "core/ProcessorState.hpp"
#include "core/Register.hpp"

#include <cstdint>

using namespace core;
using namespace core::instructions;

namespace {
  InstructionData::ParsedData generateCycl(uint32_t dest_addr) {
    return { RTypeData{ dest_addr, 0, 0 }, Opcode::CYCL };
  }
}

TEST_CASE("Start, increment, wrap", "[Instructions::Operations::Cycle]") {
  ProcessorState state;
  CHECK(state.cycle_count == 0);
  state.registers[1].set(0xDEAD'BEEF);
  CHECK(state.registers[1].get<uint32_t>() == 0xDEAD'BEEF);

  auto inst = generateCycl(1);
  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == 0);
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));

  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == 1);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));

  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == 2);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));

  CHECK(state.cycle_count == 3);

  state.cycle_count = UINT32_MAX;
  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == UINT32_MAX);
  execute(inst, state);
  CHECK(state.registers[1].get<uint32_t>() == 0);
}

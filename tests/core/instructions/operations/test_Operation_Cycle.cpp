#include <catch2/catch_test_macros.hpp>

#include "core/Instructions/Instruction.hpp"
#include "core/Instructions/InstructionData.hpp"
#include "core/ProcessorState.hpp"
#include "core/Register.hpp"

#include <cstdint>

using namespace core;
using namespace core::instructions;
using namespace core::instructions::operations;

namespace {
  Instruction generateCycl(uint32_t dest_addr) {
    const InstructionData data { Opcode::CYCL, InstructionData::Width::Word, dest_addr, {} };

    Instruction inst;
    inst.setData(data);
    return inst;
  }
}

TEST_CASE("Start, increment, wrap", "[Instructions::Operations::Cycle]") {
  ProcessorState state;
  CHECK(state.cycle_count == 0);
  state.registers[1].set(0xDEAD'BEEF);
  CHECK(state.registers[1].get<uint32_t>() == 0xDEAD'BEEF);

  auto inst = generateCycl(1);
  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0);
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));

  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 1);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));

  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 2);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));

  CHECK(state.cycle_count == 3);

  state.cycle_count = UINT32_MAX;
  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == UINT32_MAX);
  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0);
}

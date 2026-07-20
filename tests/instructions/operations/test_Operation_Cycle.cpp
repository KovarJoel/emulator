#include <catch2/catch_test_macros.hpp>

#include "Instructions/Instruction.hpp"
#include "Instructions/InstructionData.hpp"
#include "ProcessorState.hpp"
#include "Register.hpp"

#include <cstdint>

using namespace emulator::core;
using namespace emulator::core::instructions;
using namespace emulator::core::instructions::operations;

namespace {
  Instruction generateCycl(uint32_t dest_addr) {
    const InstructionData data{
      Opcode::CYCL,
      InstructionData::Width::Word,
      dest_addr,
      {}
    };

    Instruction inst;
    inst.setData(data);
    return inst;
  }
}

TEST_CASE("Start, increment, wrap", "[Instructions::Operations::Cycle]") {
  ProcessorState state;
  CHECK(state.cycle_count == 0);
  state.registers[1].set(0xDEADBEEF);
  CHECK(state.registers[1].get<uint32_t>() == 0xDEADBEEF);

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

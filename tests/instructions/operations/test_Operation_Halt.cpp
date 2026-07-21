#include <catch2/catch_test_macros.hpp>
#include <cstdint>

#include "Exceptions.hpp"
#include "Instructions/Instruction.hpp"
#include "Instructions/InstructionData.hpp"
#include "Memory.hpp"
#include "ProcessorState.hpp"
#include "RegisterBank.hpp"

using namespace emulator::core;
using namespace emulator::core::instructions;
using namespace emulator::core::instructions::operations;

namespace {
  Instruction generateHalt() {
    const InstructionData data{
      Opcode::HALT,
      InstructionData::Width::Word,
      0,
      {}
    };

    Instruction inst;
    inst.setData(data);
    return inst;
  }

  Instruction generateNop() {
    const InstructionData data{
      Opcode::ADD,
      InstructionData::Width::Word,
      0,
      {}
    };

    Instruction inst;
    inst.setData(data);
    return inst;
  }
}

TEST_CASE("Halt halts the execution", "[Instructions::Operations::Halt]") {
  ProcessorState state;

  auto inst = generateHalt();

  try {
    inst.execute(state);
    CHECK(!"should be unreachable");
  } catch(...) {}

  CHECK_THROWS_AS(inst.execute(state), exceptions::Halt);
}

TEST_CASE("State is unchanged", "[Instructions::Operations::Halt]") {
  ProcessorState state;

  for (int i = 0; auto& reg : state.registers) {
    reg.set(i++);
  }

  for (size_t i = Memory::OFFSET_CODE; i < Memory::RAM_SIZE; i += Memory::RAM_SIZE / 100) {
    state.memory.set(i, static_cast<uint32_t>(i));
  }

  auto inst = generateHalt();

  const auto oldState = state;
  CHECK_THROWS(inst.execute(state));

  CHECK(oldState.memory == state.memory);
  for (size_t i = 0; i < RegisterBank::REGISTER_COUNT; ++i) {
    constexpr size_t PC_INDEX = 15;
    if (i == PC_INDEX) {
      CHECK(oldState.registers[i] != state.registers[i]);
      CHECK(oldState.registers[i].get<uint32_t>() + inst.getData().getEncodingSize()
        == state.registers[i].get<uint32_t>());
    }
    else {
      CHECK(oldState.registers[i] == state.registers[i]);
    }
  }
  CHECK(oldState.cycle_count + 1 == state.cycle_count);
}

TEST_CASE("PC and CYCL updated", "[Instructions::Operations::Halt]") {
  const auto nop = generateNop();
  const auto halt = generateHalt();

  ProcessorState state;
  ProcessorState oldState = state;
  nop.execute(state);
  CHECK(oldState.registers.getPC().get<uint32_t>() + nop.getData().getEncodingSize()
    == state.registers.getPC().get<uint32_t>());
  CHECK(oldState.cycle_count + 1 == state.cycle_count);

  oldState = state;
  CHECK_THROWS(halt.execute(state));
  CHECK(oldState.registers.getPC().get<uint32_t>() + halt.getData().getEncodingSize()
    == state.registers.getPC().get<uint32_t>());
  CHECK(oldState.cycle_count + 1 == state.cycle_count);
}

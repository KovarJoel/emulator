#include <catch2/catch_test_macros.hpp>
#include <cstdint>

#include "core/Exceptions.hpp"
#include "core/Instructions/Instructions.hpp"
#include "core/Memory.hpp"
#include "core/ProcessorState.hpp"
#include "core/RegisterBank.hpp"

using namespace core;
using namespace core::instructions;

namespace {
  InstructionData::ParsedData generateHalt() {
    return { RTypeData { 0, 0, 0 }, Opcode::HALT };
  }

  InstructionData::ParsedData generateNop() {
    return { RTypeData { 0, 0, 0 }, Opcode::ADD };
  }
}

TEST_CASE("Halt halts the execution", "[Instructions::Operations::Halt]") {
  ProcessorState state;

  auto inst = generateHalt();

  try {
    execute(inst, state);
    CHECK(!"should be unreachable");
  } catch (...) {}

  CHECK_THROWS_AS(execute(inst, state), exceptions::Halt);
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
  CHECK_THROWS(execute(inst, state));

  CHECK(oldState.memory == state.memory);
  for (size_t i = 0; i < RegisterBank::REGISTER_COUNT; ++i) {
    constexpr size_t PC_INDEX = 15;
    if (i == PC_INDEX) {
      CHECK(oldState.registers[i] != state.registers[i]);
      CHECK(
        oldState.registers[i].get<uint32_t>() + InstructionData::ENCODING_SIZE
        == state.registers[i].get<uint32_t>()
      );
    } else {
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
  execute(nop, state);
  CHECK(
    oldState.registers.getPC().get<uint32_t>() + InstructionData::ENCODING_SIZE
    == state.registers.getPC().get<uint32_t>()
  );
  CHECK(oldState.cycle_count + 1 == state.cycle_count);

  oldState = state;
  CHECK_THROWS(execute(halt, state));
  CHECK(
    oldState.registers.getPC().get<uint32_t>() + InstructionData::ENCODING_SIZE
    == state.registers.getPC().get<uint32_t>()
  );
  CHECK(oldState.cycle_count + 1 == state.cycle_count);
}

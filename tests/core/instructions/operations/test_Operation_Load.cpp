#include <catch2/catch_test_macros.hpp>
#include <cstdint>

#include "core/Exceptions.hpp"
#include "core/Instructions/Instructions.hpp"
#include "core/ProcessorState.hpp"
#include "core/Register.hpp"

using namespace core;
using namespace core::instructions;

namespace {
  constexpr uint32_t FIRST_VALID_ADDRESS = 0x1000;
}

TEST_CASE("Valid load correctly loads", "[Instructions::Operations::Load]") {
  ProcessorState state {};
  state.memory.set<uint32_t>(FIRST_VALID_ADDRESS, 0xDEAD'BEEF);
  state.registers[2].set(FIRST_VALID_ADDRESS);

  InstructionData::ParsedData load { RTypeData { 1, 2, 0 }, Opcode::LD };
  
  execute(load, state);
  CHECK(state.registers[1].get<uint32_t>() == 0xDEAD'BEEF);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));

  load.opcode = Opcode::LDH;
  execute(load, state);
  CHECK(state.registers[1].get<uint32_t>() == 0xFFFF'BEEF);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));

  state.memory.set<uint32_t>(FIRST_VALID_ADDRESS, 0);
  load.opcode = Opcode::LD;
  execute(load, state);
  CHECK(state.registers[1].get<uint32_t>() == 0);
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
}

TEST_CASE("Valid sign/zero extension", "[Instructions::Operations::Load]") {
  ProcessorState state {};
  state.memory.set<uint32_t>(FIRST_VALID_ADDRESS, 0xDEAD'BEEF);
  state.registers[2].set(FIRST_VALID_ADDRESS);

  InstructionData::ParsedData load { RTypeData { 1, 2, 0 }, Opcode::LDB };

  execute(load, state);
  CHECK(state.registers[1].get<uint32_t>() == 0xFFFF'FFEF);

  load.opcode = Opcode::LDH;
  execute(load, state);
  CHECK(state.registers[1].get<uint32_t>() == 0xFFFF'BEEF);

  load.opcode = Opcode::LDBZX;
  execute(load, state);
  CHECK(state.registers[1].get<uint32_t>() == 0xEF);

  load.opcode = Opcode::LDHZX;
  execute(load, state);
  CHECK(state.registers[1].get<uint32_t>() == 0xBEEF);
}

TEST_CASE("Invalid read access throws", "[Instructions::Operations::Load]") {
  ProcessorState state {};
  state.registers[1].set(0);

  InstructionData::ParsedData load {
    ITypeData { 1, 1, FIRST_VALID_ADDRESS }, Opcode::LDI
  };

  CHECK_NOTHROW(execute(load, state));

  state.registers[1].set(Memory::RAM_SIZE - 1);
  std::get<ITypeData>(load.data).imm16 = 0;
  load.opcode = Opcode::LDBI;
  CHECK_NOTHROW(execute(load, state));

  state.registers[1].set(FIRST_VALID_ADDRESS - 1);
  CHECK_THROWS_AS(execute(load, state), exceptions::InvalidReadAccess);

  state.registers[1].set(Memory::RAM_SIZE);
  CHECK_THROWS_AS(execute(load, state), exceptions::InvalidReadAccess);
}

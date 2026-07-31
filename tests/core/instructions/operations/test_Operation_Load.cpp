#include <catch2/catch_test_macros.hpp>
#include <cstdint>

#include "core/Exceptions.hpp"
#include "core/Instructions/Instruction.hpp"
#include "core/Instructions/InstructionData.hpp"
#include "core/Instructions/Operations.hpp"
#include "core/Instructions/RegisterOrImmediate.hpp"
#include "core/ProcessorState.hpp"
#include "core/Register.hpp"

using namespace core;
using namespace core::instructions;

namespace {
  constexpr uint32_t FIRST_VALID_ADDRESS = 0x1000;
}

TEST_CASE("Invalid width throws", "[Instructions::Operations::Load]") {
  ProcessorState state{};
  Instruction load{};

  const std::array<RegisterOrImmediate, 2> sources{{
    { RegisterOrImmediate::SourceMode::Immediate, FIRST_VALID_ADDRESS },
    { RegisterOrImmediate::SourceMode::Register, 0 },
  }};

  using enum InstructionData::Width;
  for (const auto& width : { Byte, HalfWord, Word }) {

    for (const auto& opcode : { Opcode::LD, Opcode::LDZX }) {
      CHECK_NOTHROW(load.setData({ opcode, width, 0, sources }));
      CHECK_NOTHROW(load.execute(state));
    }
  }

  for (const auto& opcode : { Opcode::LD, Opcode::LDZX }) {
    CHECK_THROWS((
      load.setData({ opcode, InstructionData::Width{ 0b11 }, 0, sources }),
      load.execute(state)
    ));
  }
}

TEST_CASE("Invalid addressing mode throws", "[Instructions::Operations::Load]") {
  ProcessorState state{};
  Instruction load{};

  state.registers[1].set(FIRST_VALID_ADDRESS);

  std::array<RegisterOrImmediate, 2> sources{};
  
  for (const auto& opcode : { Opcode::LD, Opcode::LDZX }) {
    const auto genLoad = [&]{
      using enum InstructionData::Width;
      return Instruction{ InstructionData{ opcode, Word, 0, sources } };
    };

    sources[0].setImmediateValue(FIRST_VALID_ADDRESS);
    sources[1].setImmediateValue(FIRST_VALID_ADDRESS);
    load = genLoad();
    CHECK_THROWS_AS(load.execute(state), exceptions::InvalidAddressignMode);

    sources[0].setRegisterAddress(1);
    sources[1].setImmediateValue(FIRST_VALID_ADDRESS);
    load = genLoad();
    CHECK_THROWS_AS(load.execute(state), exceptions::InvalidAddressignMode);

    sources[0].setImmediateValue(FIRST_VALID_ADDRESS);
    sources[1].setRegisterAddress(1);
    load = genLoad();
    CHECK_NOTHROW(load.execute(state));

    sources[0].setRegisterAddress(1);
    sources[1].setRegisterAddress(1);
    load = genLoad();
    CHECK_NOTHROW(load.execute(state));
  }
}

TEST_CASE("Valid load correctly loads", "[Instructions::Operations::Load]") {
  ProcessorState state{};
  state.memory.set<uint32_t>(FIRST_VALID_ADDRESS, 0xDEADBEEF);

  const std::array<RegisterOrImmediate, 2> sources{{
    { RegisterOrImmediate::SourceMode::Immediate, FIRST_VALID_ADDRESS },
    { RegisterOrImmediate::SourceMode::Register, 0 }
  }};

  Instruction load{{ Opcode::LD, InstructionData::Width::Word, 1, sources }};
  load.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0xDEADBEEF);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));

  load.setData({ Opcode::LDZX, InstructionData::Width::Word, 2, sources});
  load.execute(state);
  CHECK(state.registers[2].get<uint32_t>() == 0xDEADBEEF);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));

  state.memory.set<uint32_t>(FIRST_VALID_ADDRESS, 0);
  load.execute(state);
  CHECK(state.registers[2].get<uint32_t>() == 0);
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
}

TEST_CASE("Valid sign/zero extension", "[Instructions::Operations::Load]") {
  ProcessorState state{};
  state.memory.set<uint32_t>(FIRST_VALID_ADDRESS, 0xDEADBEEF);

  const std::array<RegisterOrImmediate, 2> sources{{
    { RegisterOrImmediate::SourceMode::Immediate, FIRST_VALID_ADDRESS },
    { RegisterOrImmediate::SourceMode::Register, 0 }
  }};

  Instruction load{{ Opcode::LD, InstructionData::Width::Byte, 1, sources }};
  load.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0xFFFFFFEF);
  
  load.setData({ Opcode::LD, InstructionData::Width::HalfWord, 1, sources });
  load.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0xFFFFBEEF);

  load.setData({ Opcode::LDZX, InstructionData::Width::Byte, 1, sources });
  load.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0xEF);

  load.setData({ Opcode::LDZX, InstructionData::Width::HalfWord, 1, sources });
  load.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0xBEEF);
}

TEST_CASE("Invalid read access throws", "[Instructions::Operations::Load]") {
  ProcessorState state{};
  std::array<RegisterOrImmediate, 2> sources{};

  sources[0].setImmediateValue(FIRST_VALID_ADDRESS);
  Instruction load{{ Opcode::LD, InstructionData::Width::Byte, 1, sources }};
  CHECK_NOTHROW(load.execute(state));

  sources[0].setImmediateValue(Memory::RAM_SIZE - 1);
  load.setData({ Opcode::LDZX, InstructionData::Width::Byte, 1, sources });
  CHECK_NOTHROW(load.execute(state));

  sources[0].setImmediateValue(FIRST_VALID_ADDRESS - 1);
  load.setData({ Opcode::LD, InstructionData::Width::Byte, 1, sources });
  CHECK_THROWS_AS(load.execute(state), exceptions::InvalidReadAccess);

  sources[0].setImmediateValue(Memory::RAM_SIZE);
  load.setData({ Opcode::LDZX, InstructionData::Width::Byte, 1, sources });
  CHECK_THROWS_AS(load.execute(state), exceptions::InvalidReadAccess);
}

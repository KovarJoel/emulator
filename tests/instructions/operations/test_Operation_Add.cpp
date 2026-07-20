#include <catch2/catch_test_macros.hpp>

#include "Instructions/Instruction.hpp"
#include "Instructions/InstructionData.hpp"
#include "Instructions/RegisterOrImmediate.hpp"
#include "ProcessorState.hpp"
#include "Register.hpp"
#include "Types.hpp"

#include <climits>
#include <cstdint>
#include <random>
#include <algorithm>

using namespace emulator::core;
using namespace emulator::core::instructions;
using namespace emulator::core::instructions::operations;

namespace {
  ProcessorState state{};
  std::mt19937 random_engine{};
  std::uniform_int_distribution int_distribution{ INT_MIN / 4, INT_MAX / 4 };
  std::uniform_int_distribution register_distribution{ 1, 10 };
  std::uniform_int_distribution bool_distribution{ 0, 1 };

  template <EmulatorType T>
  Instruction generateAdd(uint32_t dest_addr, T value1, T value2) {
    const std::array<T, 2> values{ value1, value2 };
    std::array<RegisterOrImmediate, 2> sources{};
    for (size_t i = 0; i < sources.size(); ++i) {
      if (bool_distribution(random_engine)) {
        sources[i].setImmediateValue(values[i]);
      } else {
        size_t register_addr = 0;
        do {
          register_addr = register_distribution(random_engine);
        } while (i && sources[i - 1].getSourceMode() == InstructionData::SourceMode::Register
                && sources[i - 1].getRegisterAddress() == register_addr);
        state.registers[register_addr].set(values[i]);
        sources[i].setRegisterAddress(register_addr);
      }
    }

    const InstructionData data{
      Opcode::ADD,
      InstructionData::Width::Word,
      dest_addr,
      sources
    };

    Instruction inst;
    inst.setData(data);
    return inst;
  }
}

TEST_CASE("Random additions without overflow", "[Instructions::Operations::Add]") {
  for (size_t i = 0; i < 20; ++i) {
    const size_t dest_addr = register_distribution(random_engine);
    const int value1 = int_distribution(random_engine);
    const int value2 = int_distribution(random_engine);
    
    const int sum = value1 + value2;

    const Instruction inst = generateAdd(dest_addr, value1, value2);
    inst.execute(state);

    CHECK(state.registers[dest_addr].get<int32_t>() == sum);

    CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Zero) == (sum == 0));
    CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign) == sum < 0);
    
    const int min = std::min(value1, value2);
    const int max = std::max(value1, value2);
    CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Carry) == 
      (value1 < 0 && value2 < 0 || min < 0 && max > 0 && -max <= min)   
    );
    CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow) == 0);
  }
}

TEST_CASE("Overflow", "[Instructions::Operations::Add]") {
  Instruction inst = generateAdd(1, INT32_MAX, 1);
  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0x80000000);
  CHECK(state.registers[1].get<int32_t>() == INT32_MIN);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));

  inst = generateAdd(1, -2, -INT32_MAX);
  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0x7FFFFFFF);
  CHECK(state.registers[1].get<int32_t>() == INT_MAX);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));
}

TEST_CASE("Carry", "[Instructions::Operations::Add]") {
  Instruction inst = generateAdd(1, 0xFFFFFFFF, 1u);
  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0);
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));

  inst = generateAdd(1, 0xFFFFFFFF, 0xFFFFFFFF);
  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0xFFFFFFFE);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));

  inst = generateAdd(1, 0xFFFFFFFF, 0u);
  inst.execute(state);
  CHECK(state.registers[1].get<uint32_t>() == 0xFFFFFFFF);
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Zero));
  CHECK(state.registers.getFLAGS().getBit(Register::FlagIndex::Sign));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Carry));
  CHECK(!state.registers.getFLAGS().getBit(Register::FlagIndex::Overflow));
}

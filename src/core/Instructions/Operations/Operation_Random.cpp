#include "Operation_Random.hpp"

namespace core::instructions::operations {
  void Random::execute(const InstructionData& data, ProcessorState& state) const {
    std::uniform_int_distribution<uint32_t> distribution(
      data.getSources()[0].getValue(state.registers),
      data.getSources()[1].getValue(state.registers)
    );

    const uint32_t result = distribution(m_generator);
    state.registers[data.getDestRegisterAddr()].set<uint32_t>(result);

    updateFlagsZeroAndSign(result, state);
    advancePC(data, state);
  }
}

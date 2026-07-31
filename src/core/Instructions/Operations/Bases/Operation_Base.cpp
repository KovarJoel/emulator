#include "Operation_Base.hpp"
#include "Register.hpp"

namespace core::instructions::operations::bases {
  void Base::updateFlagsZeroAndSign(uint32_t result, ProcessorState& state) const {
    state.registers.getFLAGS().setBit(
      Register::FlagIndex::Zero,
      result == 0
    );
    
    state.registers.getFLAGS().setBit(
      Register::FlagIndex::Sign,
      result & (1u << Register::INDEX_MSB)
    );
  }

  void Base::advancePC(const InstructionData& data, ProcessorState& state) const {
    state.registers.getPC().set<uint32_t>(state.registers.getPC().get<uint32_t>() + data.getEncodingSize());
  }
}

#include "Operation_Base.hpp"

#include "Exceptions.hpp"

namespace emulator::core::instructions::operations::bases {
  void Base::updateFlagsZeroAndSign(const InstructionData& data, ProcessorState& state) const {
    if (state.registers[data.getDestRegisterAddr()].get<uint32_t>() == 0) {
      state.registers.getFLAGS().setBit(Register::FlagIndex::Zero);
    }
    if (state.registers[data.getDestRegisterAddr()].getBit(Register::INDEX_MSB)) {
      state.registers.getFLAGS().setBit(Register::FlagIndex::Sign);
    }
  }

  void Base::advancePC(const InstructionData& data, ProcessorState& state) const {
    state.registers.getPC().set<uint32_t>(state.registers.getPC().get<uint32_t>() + data.getEncodingSize());
  }
}

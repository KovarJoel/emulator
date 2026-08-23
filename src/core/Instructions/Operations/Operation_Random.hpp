#pragma once

#include "Bases/Operation_Base.hpp"

#include <random>

namespace core::instructions::operations {
  class Random : public bases::Base {
  public:
    void execute(const InstructionData& data, ProcessorState& state) const;

  private:
    static inline std::mt19937 m_generator{};
  };
}

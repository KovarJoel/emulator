#pragma once

#include "Memory.hpp"
#include "RegisterBank.hpp"

#include <cstdint>

namespace core {
  struct ProcessorState {
    Memory memory {};
    RegisterBank registers {};
    uint32_t cycle_count {};
  };
}

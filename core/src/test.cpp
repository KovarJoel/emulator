#include "Memory.hpp"

void foo() {
  using namespace emulator::core;

  Memory mem;
  mem.get<int32_t>(1234);
  mem.set(235, 100);
}
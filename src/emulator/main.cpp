#include "Emulator.hpp"

#include <cstdlib>

int main(int argc, const char** argv) {
  emulator::Emulator emulator;

  if (!emulator.setArgs(argc, argv)) {
    return EXIT_FAILURE;
  }

  emulator.run();
}

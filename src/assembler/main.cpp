#include "Assembler.hpp"
#include "AssemblerError.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, const char** argv) {

  try {
    assembler::Assembler assembler{ argc, argv };
    assembler.run();
  }
  catch (const assembler::Error& error) {
    std::cerr << error;
    return EXIT_FAILURE;
  }
}

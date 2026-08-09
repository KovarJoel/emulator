#pragma once

#include "AssemblerData.hpp"

namespace assembler {
  class Assembler {
  public:
    Assembler(int argc, const char** argv);

    void run();

  private:
    void readFile();

  private:
    AssemblerData m_data{};
  };
}

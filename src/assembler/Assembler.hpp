#pragma once

#include "AssemblerData.hpp"

#include <span>
#include <cstddef>

namespace assembler {
  class Assembler {
  public:
    Assembler(int argc, const char** argv);

    void run();

  private:
    void readFile();
    void writeFile(std::span<const std::byte> bytes);

  private:
    AssemblerData m_data{};
  };
}

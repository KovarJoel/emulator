#pragma once

#include "core/Processor.hpp"

#include <string_view>

namespace emulator {
  class Emulator {
  public:
    bool setArgs(int argc, const char** argv);
    void run();

  private:
    core::Processor m_processor{};
    std::string_view m_binary_path{};
  };
}

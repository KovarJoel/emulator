#pragma once

#include "Instructions/InstructionData.hpp"
#include "ProcessorState.hpp"

#include <cstddef>
#include <span>
#include <filesystem>

namespace emulator::core {
  class Processor {
  public:
    void loadProgram(std::span<const std::byte> binary);
    void loadProgram(const std::filesystem::path& file_path);

    void run();

    auto& getState(this auto&& self) {
      return self.m_state;
    }

  private:
    void initializeRegisters();

    instructions::InstructionData fetchAndDecode() const;
  
    private:
    ProcessorState m_state;
  };
}

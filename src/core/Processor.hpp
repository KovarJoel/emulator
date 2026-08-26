#pragma once

#include "Instructions/InstructionData.hpp"
#include "ProcessorState.hpp"

#include <cstddef>
#include <filesystem>
#include <functional>
#include <span>

namespace core {
  class Processor {
  public:
    void loadProgram(std::span<const std::byte> binary);
    void loadProgram(const std::filesystem::path& file_path);

    void run(
      std::function<void(ProcessorState& state, const instructions::InstructionData& instruction)>
        callback = [](auto&, auto&) {}
    );

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

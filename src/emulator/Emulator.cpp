#include "Emulator.hpp"

#include "core/Exceptions.hpp"
#include "core/Instructions/Operations.hpp"

#include <iostream>
#include <print>
#include <thread>

namespace emulator {
  bool Emulator::setArgs(int argc, const char** argv) {
    if (argc != 2) {
      std::println(std::cerr, "[ERROR] > Invalid number of arguments ({}), expected 2", argc);
      return false;
    }
    m_binary_path = argv[1];

    try {
      m_processor.loadProgram(m_binary_path);
    } catch ([[maybe_unused]] const core::exceptions::InvalidBinary& ex) {
      std::println(std::cerr, "[ERROR] > Failed to load binary from {}", m_binary_path);
      return false;
    }
    return true;
  }

  void Emulator::processorCallback(
    [[maybe_unused]] core::ProcessorState& state,
    const core::instructions::InstructionData& instruction
  ) {
    using enum core::instructions::Opcode;
    if (m_last_instruction.getOpcode() == ST) {
      m_app.PostEvent(ftxui::Event::Custom);
    }

    m_last_instruction = instruction;
  }

  void Emulator::run() {
    if (m_binary_path.empty()) {
      return;
    }

    std::jthread app { [&] { m_app.Loop(m_console); } };
    std::jthread processor { [&] {
      m_processor.run([this](auto&& state, auto&& instruction) {
        this->processorCallback(state, instruction);
      });
      m_app.Exit();
    } };
  }
}

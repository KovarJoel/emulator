#pragma once

#include "Components/Console.hpp"
#include "core/Instructions/InstructionData.hpp"
#include "core/Processor.hpp"

#include <string_view>

#include <ftxui/ftxui.hpp>

namespace emulator {
  class Emulator {
  public:
    bool setArgs(int argc, const char** argv);
    void run();

  private:
    void processorCallback(
      core::ProcessorState& state,
      const core::instructions::InstructionData& instruction
    );

  private:
    core::Processor m_processor {};
    std::string_view m_binary_path {};

    ftxui::App m_app { ftxui::App::FitComponent() };
    ftxui::Component m_console { ftxui::Make<Console>(m_processor.getState()) };

    core::instructions::InstructionData m_last_instruction {};
  };
}

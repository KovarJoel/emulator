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

  private:
    void initializeRegisters();

    instructions::InstructionData fetchAndDecode() const;
  
    static uint32_t fetchAndDecodeImmediate(const ProcessorState& state, size_t address);
    static instructions::InstructionData::Width decodeWidth(uint16_t remaining_data);
    static uint32_t decodeRegisterAddress(uint16_t remaining_data, size_t previous_width);
    static instructions::RegisterOrImmediate::SourceMode decodeSourceMode(uint16_t remaining_data, size_t previous_width);

    private:
    ProcessorState m_state;
  };
}

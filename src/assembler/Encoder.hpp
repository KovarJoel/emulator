#pragma once

#include "core/Instructions/RegisterOrImmediate.hpp"
#include "core/Memory.hpp"
#include "IRGenerator.hpp"

#include <cstddef>
#include <vector>

namespace assembler {
  class Encoder {
  public:
    const std::vector<std::byte>& run(const IRGenerator::Data& data);

  private:
    std::vector<std::byte> createInstructionsData();
    uint32_t createInstructionDestination(const IRGenerator::Instruction& inst);
    std::array<core::instructions::RegisterOrImmediate, 2> createInstructionSources(
      const IRGenerator::Function& func,
      const IRGenerator::Instruction& inst
    );

    core::Memory::Header createHeader(size_t code_size);
    std::vector<std::byte> createHeaderData(const core::Memory::Header& header);

    std::vector<std::byte> createVariablesData(const core::Memory::Header& header);

    uint32_t calculateDataSize() const;

    std::vector<std::byte> secondPass();

  private:
    IRGenerator::Data m_ir_data {};
    std::vector<std::byte> m_data {};
  };
}

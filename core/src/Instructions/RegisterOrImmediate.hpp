#pragma once

#include "RegisterBank.hpp"

#include <array>
#include <cstdint>

namespace emulator::core::instructions {
  class RegisterOrImmediate {
  public:
    enum class SourceMode {
      Register, Immediate
    };

  public:
    RegisterOrImmediate();
    RegisterOrImmediate(SourceMode mode, uint32_t value);

    uint32_t getRegisterAddress() const;
    uint32_t getImmediateValue() const;
    uint32_t getValue(const RegisterBank& registers) const;
    SourceMode getSourceMode() const;

    void setRegisterAddress(uint32_t register_address);
    void setImmediateValue(uint32_t immediate_value);

    bool isValid() const;

  private:
    SourceMode m_mode;
    uint32_t m_value;
  };
}

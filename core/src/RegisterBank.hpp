#pragma once

#include "Register.hpp"

#include <array>

namespace emulator::core {
  class RegisterBank {
  public:
    constexpr static size_t REGISTER_COUNT = 16;

    auto& operator[](this auto&& self, size_t index) {
      return self.m_registers[index];
    }

    auto& get0(this auto&& self) {
      return self[0];
    }

    auto& getSP(this auto&& self) {
      return self[13];
    }

    auto& getFLAGS(this auto&& self) {
      return self[14];
    }

    auto& getPC(this auto&& self) {
      return self[15];
    }

  private:
    std::array<Register, REGISTER_COUNT> m_registers;
  };
}

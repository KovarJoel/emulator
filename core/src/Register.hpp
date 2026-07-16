#pragma once

#include "Types.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <utility>

namespace emulator::core {
  class Register {
  public:
    enum class FlagIndex {
      Zero, Sign, Carry, Overflow, COUNT
    };

  public:
    template <EmulatorType T>
    auto get() const {
      T value{};
      std::memcpy(&value, m_data.data(), sizeof value);
      return value;
    }

    template <EmulatorType T>
    void set(T value) {
      std::memcpy(m_data.data(), &value, sizeof value);

      if constexpr (sizeof(T) < sizeof m_data) {
        const std::byte sign_extended = static_cast<std::byte>(value < 0 ? 0xFF : 0x00);
        for (size_t i = sizeof value; i < sizeof m_data; ++i) {
          m_data[i] = sign_extended;
        }
      }
    }

    bool getBit(size_t index) const {
      return get<uint32_t>() & (1u << index);
    }

    bool getBit(FlagIndex index) const {
      return getBit(std::to_underlying(index));
    }

    void setBit(size_t index, bool value = true) {
      set<uint32_t>(
        get<uint32_t>() & ~(1u << index) | (static_cast<unsigned>(value) << index)
      );
    }

    bool setBit(FlagIndex index, bool value = true) {
      setBit(std::to_underlying(index));
    }

    void toggleBit(size_t index) {
      set<uint32_t>(
        get<uint32_t>() ^ (1u << index)
      );
    }

    void toggleBit(FlagIndex index) {
      toggleBit(std::to_underlying(index));
    }

  private:
    std::array<std::byte, 4> m_data{};
  };
}

#pragma once

#include "Types.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <utility>
#include <type_traits>

namespace core {
  class Register {
  public:
    enum class FlagIndex {
      Zero, Sign, Carry, Overflow
    };

    struct FixAtZero {};

    constexpr static size_t INDEX_MSB = 31;
    constexpr static size_t INDEX_LSB = 0;

  public:
    Register() = default;
    Register(FixAtZero)
      : m_data{}, m_fix_at_zero{ true } {
    }
    Register(const Register& other)
      : m_data( other.m_data ), m_fix_at_zero() {
    }
    Register(Register&& other) noexcept
      : m_data{ other.m_data }, m_fix_at_zero{} {
    }
    Register& operator=(const Register& other) {
      m_data = other.m_data;
      return *this;
    }
    Register& operator=(Register&& other) noexcept {
      m_data = other.m_data;
      return *this;
    }

    template <EmulatorType T>
    auto get() const {
      if (m_fix_at_zero) return T{ 0 };

      T value{};
      std::memcpy(&value, m_data.data(), sizeof value);
      return value;
    }

    template <EmulatorType T>
    void set(T value) {
      if (m_fix_at_zero) return;

      using Extended = std::conditional_t<std::is_signed_v<T>, std::int32_t, std::uint32_t>;
      const Extended extended = value;

      std::memcpy(m_data.data(), &extended, sizeof extended);
    }

    bool getBit(size_t index) const {
      return get<uint32_t>() & (1u << index);
    }

    bool getBit(FlagIndex index) const {
      return getBit(std::to_underlying(index));
    }

    void setBit(size_t index, bool value = true) {
      set<uint32_t>(
        (get<uint32_t>() & ~(1u << index)) | (static_cast<unsigned>(value) << index)
      );
    }

    void setBit(FlagIndex index, bool value = true) {
      setBit(std::to_underlying(index), value);
    }

    void toggleBit(size_t index) {
      set<uint32_t>(
        get<uint32_t>() ^ (1u << index)
      );
    }

    void toggleBit(FlagIndex index) {
      toggleBit(std::to_underlying(index));
    }

    bool operator==(const Register& other) const {
      return m_data == other.m_data;
    }

  private:
    std::array<std::byte, 4> m_data{};
    bool m_fix_at_zero{};
  };
}

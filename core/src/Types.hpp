#pragma once

#include <concepts>
#include <cstdint>
#include <variant>

namespace emulator::core {
  template <typename T>
  concept EmulatorType =
    std::same_as<T, uint8_t> ||
    std::same_as<T, int8_t> ||
    std::same_as<T, uint16_t> ||
    std::same_as<T, int16_t> ||
    std::same_as<T, uint32_t> ||
    std::same_as<T, int32_t>;
    
  using EmulatorVariant =
    std::variant<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t>;

  static_assert(std::is_trivially_copyable_v<uint8_t>);
  static_assert(std::is_trivially_copyable_v<int8_t>);
  static_assert(std::is_trivially_copyable_v<uint16_t>);
  static_assert(std::is_trivially_copyable_v<int16_t>);
  static_assert(std::is_trivially_copyable_v<uint32_t>);
  static_assert(std::is_trivially_copyable_v<int32_t>);
}

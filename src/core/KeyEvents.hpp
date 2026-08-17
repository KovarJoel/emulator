#pragma once

#include <array>
#include <cstdint>

namespace core {
  struct KeyEvents {
    std::array<int8_t, 256> event_buffer{};
    uint32_t begin{};
    uint32_t size{};

    bool enqueueEvent(int8_t event);
    int8_t dequeueEvent();
    void clear();
  };
}

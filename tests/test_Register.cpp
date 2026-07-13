#include <catch2/catch_test_macros.hpp>

#include "Register.hpp"

using namespace emulator::core;

TEST_CASE("Truncation", "[Register]") {
  Register reg;
  CHECK(reg.get<int>() == 0);

  reg.set<uint32_t>(-1);
  CHECK(reg.get<uint8_t>() == 0xFF);
  CHECK(reg.get<uint16_t>() == 0xFFFF);
  CHECK(reg.get<uint32_t>() == 0xFFFFFFFF);
  CHECK(reg.get<int8_t>() == -1);
  CHECK(reg.get<int16_t>() == -1);
  CHECK(reg.get<int32_t>() == -1);

  reg.set<uint32_t>(0x01234567);
  CHECK(reg.get<uint8_t>() == 0x67);
  CHECK(reg.get<uint16_t>() == 0x4567);
  CHECK(reg.get<uint32_t>() == 0x01234567);
  CHECK(reg.get<int8_t>() == 0x67);
  CHECK(reg.get<int16_t>() == 0x4567);
  CHECK(reg.get<int32_t>() == 0x01234567);

  reg.set<uint32_t>(0xDEADBEEF);
  CHECK(reg.get<uint8_t>() == 0xEF);
  CHECK(reg.get<uint16_t>() == 0xBEEF);
  CHECK(reg.get<uint32_t>() == 0xDEADBEEF);
  CHECK(reg.get<int8_t>() == 0xEF - (1 << 8));
  CHECK(reg.get<int16_t>() == 0xBEEF - (1 << 16));
  CHECK(reg.get<int32_t>() == 0xDEADBEEFll - (1ll << 32));
}

TEST_CASE("Extension", "[Register]") {
  Register reg;

  reg.set<int8_t>(-1);
  CHECK(reg.get<int8_t>() == -1);
  CHECK(reg.get<int16_t>() == -1);
  CHECK(reg.get<int32_t>() == -1);
  CHECK(reg.get<uint8_t>() == 0xFF);
  CHECK(reg.get<uint16_t>() == 0xFFFF);
  CHECK(reg.get<uint32_t>() == 0xFFFFFFFF);

  reg.set<int16_t>(-16657); // 0xBEEF
  CHECK(reg.get<int8_t>() == -17);
  CHECK(reg.get<int16_t>() == -16657);
  CHECK(reg.get<int32_t>() == -16657);
  CHECK(reg.get<uint8_t>() == 0xEF);
  CHECK(reg.get<uint16_t>() == 0xBEEF);
  CHECK(reg.get<uint32_t>() == 0xFFFFBEEF);

  reg.set<uint8_t>(0xEF);
  CHECK(reg.get<int8_t>() == -17);
  CHECK(reg.get<int16_t>() == 0xEF);
  CHECK(reg.get<int32_t>() == 0xEF);
  CHECK(reg.get<uint8_t>() == 0xEF);
  CHECK(reg.get<uint16_t>() == 0xEF);
  CHECK(reg.get<uint32_t>() == 0xEF);

  reg.set<uint16_t>(0xBEEF);
  CHECK(reg.get<uint16_t>() == 0xBEEF);
  CHECK(reg.get<uint32_t>() == 0xBEEF);
  CHECK(reg.get<int16_t>() == -16657);
  CHECK(reg.get<int32_t>() == 0xBEEF);
}

TEST_CASE("Bit Operations", "[Register]") {
  Register reg;
  CHECK(reg.get<uint32_t>() == 0);

  reg.toggleBit(0);
  CHECK(reg.get<uint32_t>() == 1);
  reg.toggleBit(0);
  CHECK(reg.get<uint32_t>() == 0);

  reg.setBit(0, 0);
  CHECK(reg.getBit(0) == 0);
  CHECK(reg.get<uint32_t>() == 0);
  reg.setBit(0);
  CHECK(reg.get<uint32_t>() == 1);
  CHECK(reg.getBit(0) == 1);

  constexpr uint32_t val = 0b1010'1010'1010'1010'1010'1010'1010'1010;
  reg.set<uint32_t>(val);
  CHECK(reg.get<uint32_t>() == val);
  for (size_t i = 0; i < 32; ++i) {
    CHECK(reg.getBit(i) == (i % 2));
  }
  for (size_t i = 0; i < 32; ++i) {
    reg.toggleBit(i);
  }
  CHECK(reg.get<uint32_t>() == ~val);
  for (size_t i = 0; i < 32; ++i) {
    reg.setBit(i, !reg.getBit(i));
  }
  CHECK(reg.get<uint32_t>() == val);
}

#pragma once

namespace assembler {
  bool isBaseSymbol(char ch);

  constexpr const char* DIGITS_HEX = "0123456789ABCDEFabcdef";
  constexpr const char* DIGITS_DEC = "0123456789";
  constexpr const char* DIGITS_OCT = "01234567";
  constexpr const char* DIGITS_BIN = "01";
}

#include "Utility.hpp"

#include <cctype>

namespace assembler {
  bool isBaseSymbol(char symbol) {
    symbol = std::tolower(symbol);
    return symbol == 'x' || symbol == 'o' || symbol == 'b';
  }
}

#include "Utility.hpp"

#include <cctype>

namespace assembler {
  bool isBaseSymbol(char symbol) {
    symbol = static_cast<char>(std::tolower(static_cast<unsigned char>(symbol)));
    return symbol == 'x' || symbol == 'o' || symbol == 'b';
  }
}

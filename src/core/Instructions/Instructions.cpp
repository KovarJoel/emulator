#include "Instructions.hpp"

namespace core::instructions {
  EncodingType opcodeGetEncodingType(const Opcode& code) {
    switch (code) {
      #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, class, ...) case Opcode::mnemonic: return GetEncodingType_v<class::DecodedType>;
        EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
      #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
    }
    throw exceptions::InvalidOperation{};
  }
}

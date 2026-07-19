#include "RegisterOrImmediate.hpp"

#include "Exceptions.hpp"

namespace emulator::core::instructions {
  RegisterOrImmediate::RegisterOrImmediate()
    : m_mode{ SourceMode::Register }, m_value{ 0 } {
    if (!isValid()) {
      throw exceptions::BadRegisterOrImmediateValue{};
    }
  }
  
  RegisterOrImmediate::RegisterOrImmediate(SourceMode mode, uint32_t value)
    : m_mode{ mode }, m_value{ value } {
    if (!isValid()) {
      throw exceptions::BadRegisterOrImmediateValue{};
    }
  }

  uint32_t RegisterOrImmediate::getRegisterAddress() const {
    if (m_mode != SourceMode::Register) {
      throw exceptions::BadRegisterOrImmediateCast{};
    }
    return m_value;
  }

  uint32_t RegisterOrImmediate::getImmediateValue() const {
    if (m_mode != SourceMode::Immediate) {
      throw exceptions::BadRegisterOrImmediateCast{};
    }
    return m_value;
  }
  
  uint32_t RegisterOrImmediate::getValue(const RegisterBank& registers) const {
    switch (m_mode) {
      case SourceMode::Register:
        return registers[m_value].get<uint32_t>();
      case SourceMode::Immediate:
        return m_value;
    }
    throw exceptions::BadRegisterOrImmediateValue{};
  }

  RegisterOrImmediate::SourceMode RegisterOrImmediate::getSourceMode() const {
    return m_mode;
  }
  
  void RegisterOrImmediate::setRegisterAddress(uint32_t register_address) {
    RegisterOrImmediate temp{ SourceMode::Register, register_address };
    *this = temp;
  }
  
  void RegisterOrImmediate::setImmediateValue(uint32_t immediate_value) {
    RegisterOrImmediate temp{ SourceMode::Immediate, immediate_value };
    *this = temp;
  }

  bool RegisterOrImmediate::isValid() const {
    switch (m_mode) {
      case SourceMode::Register:
        return m_value < RegisterBank::REGISTER_COUNT;
      case SourceMode::Immediate:
        return true;
    }
    return false;
  }
}

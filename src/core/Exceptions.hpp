#pragma once

#include <exception>
#include <format>
#include <source_location>
#include <string>

#ifndef EMULATOR_CORE_EXCEPTIONS_USE_LOCATION
  #define EMULATOR_CORE_EXCEPTIONS_USE_LOCATION 0
#endif

#define EMULATOR_CORE_EXCEPTION_CREATE(Name, Base)                            \
  struct Name : Base {                                                        \
    Name(                                                                     \
      const std::source_location& location = std::source_location::current(), \
      const std::string& name = #Name                                         \
    )                                                                         \
      : Base { location, name } {}                                            \
  }

namespace core::exceptions {
  struct Base : std::exception {
    Base(
      [[maybe_unused]] const std::source_location& location = std::source_location::current(),
      const std::string& name = "Unknown"
    ) {
#if EMULATOR_CORE_EXCEPTIONS_USE_LOCATION
      m_information = std::format("{} [core::exceptions] at {}", name, locationToString(location));
#else
      m_information = std::format("{} [core::exceptions]", name);
#endif
    }

    const char* what() const noexcept override {
      return m_information.c_str();
    }

  protected:
    static std::string locationToString(const std::source_location& location) {
      return std::format(
        "{}:{}:{} in function {}",
        location.file_name(),
        location.line(),
        location.column(),
        location.function_name()
      );
    }

  protected:
    std::string m_information {};
  };

  EMULATOR_CORE_EXCEPTION_CREATE(InvalidMemoryAccess, Base);
  EMULATOR_CORE_EXCEPTION_CREATE(InvalidReadAccess, InvalidMemoryAccess);
  EMULATOR_CORE_EXCEPTION_CREATE(InvalidWriteAccess, InvalidMemoryAccess);
  EMULATOR_CORE_EXCEPTION_CREATE(InvalidExecuteAccess, InvalidMemoryAccess);

  EMULATOR_CORE_EXCEPTION_CREATE(InvalidOperation, Base);
  EMULATOR_CORE_EXCEPTION_CREATE(InvalidOperationData, Base);

  EMULATOR_CORE_EXCEPTION_CREATE(InvalidAddressignMode, Base);

  EMULATOR_CORE_EXCEPTION_CREATE(InvalidOperationWidth, Base);

  EMULATOR_CORE_EXCEPTION_CREATE(StackOverflow, Base);

  EMULATOR_CORE_EXCEPTION_CREATE(InvalidBinary, Base);

  EMULATOR_CORE_EXCEPTION_CREATE(DivideByZero, Base);

  EMULATOR_CORE_EXCEPTION_CREATE(Halt, Base);

  EMULATOR_CORE_EXCEPTION_CREATE(BadRegisterOrImmediateCast, Base);
  EMULATOR_CORE_EXCEPTION_CREATE(BadRegisterOrImmediateValue, Base);

  EMULATOR_CORE_EXCEPTION_CREATE(BadInstructionValue, Base);
}

#undef EMULATOR_CORE_EXCEPTION_CREATE

#pragma once

#include <stdexcept>

namespace emulator::core::exceptions {
  struct Base : public std::exception {};
  
  struct InvalidMemoryAccess : public Base {};
  struct InvalidReadAccess : public InvalidMemoryAccess{};
  struct InvalidWriteAccess : public InvalidMemoryAccess{};
  struct InvalidExecuteAccess : public InvalidMemoryAccess{};

  struct InvalidOperation : public Base {};
  struct InvalidOperationData : public Base{};
  
  struct InvalidAddressignMode : public Base{};

  struct InvalidOperationWidth : public Base{};

  struct StackOverflow : public Base{};

  struct InvalidBinary : public Base{};

  struct DivideByZero : public Base{};

  struct Halt : public Base{};

  struct BadRegisterOrImmediateCast : public Base {};
  struct BadRegisterOrImmediateValue : public Base {};
  
  struct BadInstructionValue : public Base {};
}

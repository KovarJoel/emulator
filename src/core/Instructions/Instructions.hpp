#pragma once

#include "../Processor.hpp"
#include "InstructionData.hpp"

#include <climits>
#include <random>

namespace core::instructions {
  class InstructionBase {
  protected:
    constexpr static bool getWordBit(std::uint32_t word, std::size_t bit) noexcept {
      return word & (1u << bit);
    }

    inline static void updateFlagsZeroAndSign(std::uint32_t result, ProcessorState& state) noexcept {
      auto& flags = state.registers.getFLAGS();
      flags.setBit(Register::FlagIndex::Zero, result == 0);
      flags.setBit(Register::FlagIndex::Sign, getWordBit(result, Register::INDEX_MSB));
    }

    inline static void advancePC(ProcessorState& state) noexcept {
      auto& pc = state.registers.getPC();
      pc.set<std::uint32_t>(pc.get<std::uint32_t>() + InstructionData::ENCODING_SIZE);
    }
  };

  class InstructionBaseLoad : public InstructionBase {
  protected:
    template <typename DecodedType, typename T>
    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      
      std::uint32_t address = 0;
      if constexpr (std::is_same_v<DecodedType, RTypeData>) {
        address = state.registers[decoded.reg2_addr].template get<std::uint32_t>()
          + state.registers[decoded.reg3_addr].template get<std::uint32_t>() * sizeof(T);
      } else if constexpr (std::is_same_v<DecodedType, ITypeData>) {
        address = state.registers[decoded.reg2_addr].template get<std::uint32_t>() + static_cast<std::int32_t>(decoded.imm16);
      } else {
        static_assert(false);
      }
      
      std::conditional_t<std::is_signed_v<T>, std::int32_t, std::uint32_t> result = 0;
      {
        std::lock_guard<std::mutex> lock { state.memory.getMutex() };
        result = state.memory.get<T>(address);
      }
      state.registers[decoded.reg1_addr].set(result);
      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class Load final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LD;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::uint32_t>(data, state);
    }
  };

  class LoadHalfWord final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LDH;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::int16_t>(data, state);
    }
  };

  class LoadHalfWordZeroExtend final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LDHZX;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::uint16_t>(data, state);
    }
  };

  class LoadByte final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LDB;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::int8_t>(data, state);
    }
  };

  class LoadByteZeroExtend final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LDBZX;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::uint8_t>(data, state);
    }
  };

  class LoadDisplaced final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LDI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::uint32_t>(data, state);
    }
  };

  class LoadHalfWordDisplaced final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LDHI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::int16_t>(data, state);
    }
  };

  class LoadHalfWordZeroExtendDisplaced final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LDHZXI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::uint16_t>(data, state);
    }
  };

  class LoadByteDisplaced final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LDBI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::int8_t>(data, state);
    }
  };

  class LoadByteZeroExtendDisplaced final : public InstructionBaseLoad {
  public:
    constexpr static Opcode opcode = Opcode::LDBZXI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseLoad::execute<DecodedType, std::uint8_t>(data, state);
    }
  };

  class InstructionBaseStore : public InstructionBase {
  protected:
    template <typename DecodedType, typename T>
    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      
      const T value = state.registers[decoded.reg1_addr].template get<T>();
      
      std::uint32_t address = 0;
      if constexpr (std::is_same_v<DecodedType, RTypeData>) {
        address = state.registers[decoded.reg2_addr].template get<std::uint32_t>()
          + state.registers[decoded.reg3_addr].template get<std::uint32_t>() * sizeof(T);
      } else if constexpr (std::is_same_v<DecodedType, ITypeData>) {
        address = state.registers[decoded.reg2_addr].template get<std::uint32_t>() + static_cast<std::int32_t>(decoded.imm16);
      } else {
        static_assert(false);
      }
      
      {
        std::lock_guard<std::mutex> lock { state.memory.getMutex() };
        state.memory.set(address, value);
      }
      advancePC(state);
    }
  };

  class Store final : public InstructionBaseStore {
  public:
    constexpr static Opcode opcode = Opcode::ST;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseStore::execute<DecodedType, std::uint32_t>(data, state);
    }
  };

  class StoreHalfWord final : public InstructionBaseStore {
  public:
    constexpr static Opcode opcode = Opcode::STH;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseStore::execute<DecodedType, std::uint16_t>(data, state);
    }
  };

  class StoreByte final : public InstructionBaseStore {
  public:
    constexpr static Opcode opcode = Opcode::STB;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseStore::execute<DecodedType, std::uint8_t>(data, state);
    }
  };

  class StoreDisplaced final : public InstructionBaseStore {
  public:
    constexpr static Opcode opcode = Opcode::STI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseStore::execute<DecodedType, std::uint32_t>(data, state);
    }
  };

  class StoreHalfWordDisplaced final : public InstructionBaseStore {
  public:
    constexpr static Opcode opcode = Opcode::STHI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseStore::execute<DecodedType, std::uint16_t>(data, state);
    }
  };

  class StoreByteDisplaced final : public InstructionBaseStore {
  public:
    constexpr static Opcode opcode = Opcode::STBI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      InstructionBaseStore::execute<DecodedType, std::uint8_t>(data, state);
    }
  };

  class InstructionBaseAdd : public InstructionBase {
  protected:
    static void execute(std::size_t dest_reg_addr, std::uint64_t value1, std::uint64_t value2, ProcessorState& state) {
      const std::uint64_t result = value1 + value2;

      state.registers[dest_reg_addr].set(static_cast<std::uint32_t>(result));

      auto& flags = state.registers.getFLAGS();
      flags.setBit(Register::FlagIndex::Carry, result > std::numeric_limits<std::uint32_t>::max());

      const bool value1_signed = getWordBit(static_cast<uint32_t>(value1), Register::INDEX_MSB);
      const bool value2_signed = getWordBit(static_cast<uint32_t>(value2), Register::INDEX_MSB);
      const bool result_signed = getWordBit(static_cast<uint32_t>(result), Register::INDEX_MSB);
      state.registers.getFLAGS().setBit(
        Register::FlagIndex::Overflow,
        value1_signed == value2_signed && value1_signed != result_signed
      );

      updateFlagsZeroAndSign(static_cast<std::uint32_t>(result), state);
      advancePC(state);
    }
  };

  class Add final : public InstructionBaseAdd {
  public:
    constexpr static Opcode opcode = Opcode::ADD;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      
      const std::uint64_t value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const std::uint64_t value2 = state.registers[decoded.reg3_addr].get<std::uint32_t>();
      
      InstructionBaseAdd::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class AddImmediate final : public InstructionBaseAdd {
  public:
    constexpr static Opcode opcode = Opcode::ADDI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      
      const std::uint64_t value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const std::uint64_t value2 = static_cast<std::uint32_t>(static_cast<std::int32_t>(decoded.imm16));
      
      InstructionBaseAdd::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class InstructionBaseSubtract : public InstructionBase {
  protected:
    static void execute(std::size_t dest_reg_addr, std::uint64_t value1, std::uint64_t value2, ProcessorState& state) {
      const std::uint64_t result = value1 - value2;

      state.registers[dest_reg_addr].set(static_cast<std::uint32_t>(result));

      auto& flags = state.registers.getFLAGS();
      flags.setBit(Register::FlagIndex::Carry, value1 < value2);

      const bool value1_signed = getWordBit(static_cast<uint32_t>(value1), Register::INDEX_MSB);
      const bool value2_signed = getWordBit(static_cast<uint32_t>(value2), Register::INDEX_MSB);
      const bool result_signed = getWordBit(static_cast<uint32_t>(result), Register::INDEX_MSB);
      state.registers.getFLAGS().setBit(
        Register::FlagIndex::Overflow,
        value1_signed != value2_signed && value1_signed != result_signed
      );

      updateFlagsZeroAndSign(static_cast<std::uint32_t>(result), state);
      advancePC(state);
    }
  };

  class Subtract final : public InstructionBaseSubtract {
  public:
    constexpr static Opcode opcode = Opcode::SUB;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      
      const std::uint64_t value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const std::uint64_t value2 = state.registers[decoded.reg3_addr].get<std::uint32_t>();
      
      InstructionBaseSubtract::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class SubtractImmediate final : public InstructionBaseSubtract {
  public:
    constexpr static Opcode opcode = Opcode::SUBI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      
      const std::uint64_t value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const std::uint64_t value2 = static_cast<std::uint32_t>(static_cast<std::int32_t>(decoded.imm16));
      
      InstructionBaseSubtract::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class InstructionBaseMultiply : public InstructionBase {
  protected:
    static void execute(std::size_t dest_reg_addr, std::uint64_t value1, std::uint64_t value2, ProcessorState& state) {
      const uint64_t result = value1 * value2;

      state.registers[dest_reg_addr].set<uint32_t>(static_cast<uint32_t>(result));

      state.registers.getFLAGS().setBit(
        Register::FlagIndex::Carry,
        result > std::numeric_limits<uint32_t>::max()
      );

      const int64_t signed_result = static_cast<int64_t>(static_cast<int32_t>(value1)) * static_cast<int64_t>(static_cast<int32_t>(value2));
      state.registers.getFLAGS().setBit(
        Register::FlagIndex::Overflow,
        signed_result < INT32_MIN || signed_result > INT32_MAX
      );

      updateFlagsZeroAndSign(static_cast<uint32_t>(result), state);
      advancePC(state);
    }
  };

  class Multiply final : public InstructionBaseMultiply {
  public:
    constexpr static Opcode opcode = Opcode::MUL;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = state.registers[decoded.reg3_addr].get<std::uint32_t>();
      InstructionBaseMultiply::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class MultiplyImmediate final : public InstructionBaseMultiply {
  public:
    constexpr static Opcode opcode = Opcode::MULI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = static_cast<std::uint32_t>(static_cast<std::int32_t>(decoded.imm16));
      InstructionBaseMultiply::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class InstructionBaseDiv : public InstructionBase {
  protected:
    template <typename T>
    static void execute(std::size_t dest_reg_addr, T value1, T value2, ProcessorState& state) {
      if (value2 == 0) {
        throw exceptions::DivideByZero {};
      }

      const auto result = value1 / value2;
      state.registers[dest_reg_addr].set(result);

      if constexpr (std::is_signed_v<T>) {
        updateFlagsZeroAndSign(result, state);
      } else {
        state.registers.getFLAGS().setBit(Register::FlagIndex::Zero, result == 0);
      }
      advancePC(state);
    }
  };

  class Divide final : public InstructionBaseDiv {
  public:
    constexpr static Opcode opcode = Opcode::DIV;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::int32_t>();
      const auto value2 = state.registers[decoded.reg3_addr].get<std::int32_t>();
      InstructionBaseDiv::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class DivideImmediate final : public InstructionBaseDiv {
  public:
    constexpr static Opcode opcode = Opcode::DIVI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const std::int32_t value1 = state.registers[decoded.reg2_addr].get<std::int32_t>();
      const std::int32_t value2 = decoded.imm16;
      InstructionBaseDiv::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class DivideUnsigned final : public InstructionBaseDiv {
  public:
    constexpr static Opcode opcode = Opcode::DIVU;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = state.registers[decoded.reg3_addr].get<std::uint32_t>();
      InstructionBaseDiv::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class DivideUnsignedImmediate final : public InstructionBaseDiv {
  public:
    constexpr static Opcode opcode = Opcode::DIVUI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = static_cast<std::uint32_t>(decoded.imm16);
      InstructionBaseDiv::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class InstructionBaseRemainder : public InstructionBase {
  protected:
    template <typename T>
    static void execute(std::size_t dest_reg_addr, T value1, T value2, ProcessorState& state) {
      if (value2 == 0) {
        throw exceptions::DivideByZero {};
      }

      const auto result = value1 % value2;
      state.registers[dest_reg_addr].set(result);

      if constexpr (std::is_signed_v<T>) {
        updateFlagsZeroAndSign(result, state);
      } else {
        state.registers.getFLAGS().setBit(Register::FlagIndex::Zero, result == 0);
      }
      advancePC(state);
    }
  };

  class Remainder final : public InstructionBaseRemainder {
  public:
    constexpr static Opcode opcode = Opcode::REM;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::int32_t>();
      const auto value2 = state.registers[decoded.reg3_addr].get<std::int32_t>();
      InstructionBaseRemainder::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class RemainderImmediate final : public InstructionBaseRemainder {
  public:
    constexpr static Opcode opcode = Opcode::REMI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::int32_t>();
      const auto value2 = static_cast<std::int32_t>(decoded.imm16);
      InstructionBaseRemainder::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class RemainderUnsigned final : public InstructionBaseRemainder {
  public:
    constexpr static Opcode opcode = Opcode::REMU;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = state.registers[decoded.reg3_addr].get<std::uint32_t>();
      InstructionBaseRemainder::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class RemainderUnsignedImmediate final : public InstructionBaseRemainder {
  public:
    constexpr static Opcode opcode = Opcode::REMUI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = static_cast<std::uint32_t>(decoded.imm16);
      InstructionBaseRemainder::execute(decoded.reg1_addr, value1, value2, state);
    }
  };

  class RotateLeft final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::ROL;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = state.registers[decoded.reg3_addr].get<std::uint32_t>();

      const auto result = std::rotl(value1, value2 % (CHAR_BIT * sizeof value1));
      state.registers[decoded.reg1_addr].set(result);

      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class RotateLeftImmediate final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::ROLI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = static_cast<std::uint32_t>(decoded.imm16);

      const auto result = std::rotl(value1, value2 % (CHAR_BIT * sizeof value1));
      state.registers[decoded.reg1_addr].set(result);
      
      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class RotateRight final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::ROR;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = state.registers[decoded.reg3_addr].get<std::uint32_t>();

      const auto result = std::rotr(value1, value2 % (CHAR_BIT * sizeof value1));
      state.registers[decoded.reg1_addr].set(result);
      
      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class RotateRightImmediate final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::RORI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto value1 = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto value2 = static_cast<std::uint32_t>(decoded.imm16);

      const auto result = std::rotr(value1, value2 % (CHAR_BIT * sizeof value1));
      state.registers[decoded.reg1_addr].set(result);
      
      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class InstructionBaseShift : public InstructionBase {
  protected:
    static void updateFlagOverflow(std::uint32_t src, std::uint32_t dest, ProcessorState& state) {
      state.registers.getFLAGS().setBit(
        Register::FlagIndex::Overflow,
        getWordBit(src, Register::INDEX_MSB) != getWordBit(dest, Register::INDEX_MSB)
      );
    }
  };

  class ShiftLeftLogical final : public InstructionBaseShift {
  public:
    constexpr static Opcode opcode = Opcode::SLL;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto src = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto positions = state.registers[decoded.reg3_addr].get<std::uint32_t>();
      const auto result = positions >= CHAR_BIT * sizeof src ? 0 : src << positions;

      state.registers[decoded.reg1_addr].set(result);

      if (positions != 0) {
        state.registers.getFLAGS().setBit(
          Register::FlagIndex::Carry,
          positions >= CHAR_BIT * sizeof src ? 0 : getWordBit(src, 32 - positions)
        );
      }

      updateFlagsZeroAndSign(result, state);
      updateFlagOverflow(src, result, state);
      advancePC(state);
    }
  };

  class ShiftLeftLogicalImmediate final : public InstructionBaseShift {
  public:
    constexpr static Opcode opcode = Opcode::SLLI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto src = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto positions = static_cast<std::uint32_t>(decoded.imm16);
      const auto result = positions >= CHAR_BIT * sizeof src ? 0 : src << positions;

      state.registers[decoded.reg1_addr].set(result);

      if (positions != 0) {
        state.registers.getFLAGS().setBit(
          Register::FlagIndex::Carry,
          positions >= CHAR_BIT * sizeof src ? 0 : getWordBit(src, 32 - positions)
        );
      }

      updateFlagsZeroAndSign(result, state);
      updateFlagOverflow(src, result, state);
      advancePC(state);
    }
  };

  class ShiftRightLogical final : public InstructionBaseShift {
  public:
    constexpr static Opcode opcode = Opcode::SRL;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto src = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto positions = state.registers[decoded.reg3_addr].get<std::uint32_t>();
      const auto result = positions >= CHAR_BIT * sizeof src ? 0 : src >> positions;

      state.registers[decoded.reg1_addr].set(result);

      if (positions != 0) {
        state.registers.getFLAGS().setBit(
          Register::FlagIndex::Carry,
          positions >= CHAR_BIT * sizeof src ? 0 : getWordBit(src, positions - 1)
        );
      }

      updateFlagsZeroAndSign(result, state);
      updateFlagOverflow(src, result, state);
      advancePC(state);
    }
  };

  class ShiftRightLogicalImmediate final : public InstructionBaseShift {
  public:
    constexpr static Opcode opcode = Opcode::SRLI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto src = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto positions = static_cast<std::uint32_t>(decoded.imm16);
      const auto result = positions >= CHAR_BIT * sizeof src ? 0 : src >> positions;

      state.registers[decoded.reg1_addr].set(result);

      if (positions != 0) {
        state.registers.getFLAGS().setBit(
          Register::FlagIndex::Carry,
          positions >= CHAR_BIT * sizeof src ? 0 : getWordBit(src, positions - 1)
        );
      }

      updateFlagsZeroAndSign(result, state);
      updateFlagOverflow(src, result, state);
      advancePC(state);
    }
  };

  class ShiftRightArithmetic final : public InstructionBaseShift {
  public:
    constexpr static Opcode opcode = Opcode::SRA;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto src = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto positions = state.registers[decoded.reg3_addr].get<std::uint32_t>();
      const auto result = positions >= CHAR_BIT * sizeof src
        ? static_cast<std::uint32_t>(-1)
        : (src >> positions) | ((~(1u << (positions - 1))) << (CHAR_BIT * sizeof src - positions));
      // TODO: Check if this makes any sense??? What about unsigned values?

      state.registers[decoded.reg1_addr].set(result);

      if (positions != 0) {
        state.registers.getFLAGS().setBit(
          Register::FlagIndex::Carry,
          positions >= CHAR_BIT * sizeof src ? 0 : getWordBit(src, positions - 1)
        );
      }

      updateFlagsZeroAndSign(result, state);
      updateFlagOverflow(src, result, state);
      advancePC(state);
    }
  };

  class ShiftRightArithmeticImmediate final : public InstructionBaseShift {
  public:
    constexpr static Opcode opcode = Opcode::SRAI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto src = state.registers[decoded.reg2_addr].get<std::uint32_t>();
      const auto positions = static_cast<std::uint32_t>(decoded.imm16);
      const auto result = positions >= CHAR_BIT * sizeof src
        ? static_cast<std::uint32_t>(-1)
        : (src >> positions) | ((~(1u << (positions - 1))) << (CHAR_BIT * sizeof src - positions));
      // TODO: Check if this makes any sense??? What about unsigned values?

      state.registers[decoded.reg1_addr].set(result);

      if (positions != 0) {
        state.registers.getFLAGS().setBit(
          Register::FlagIndex::Carry,
          positions >= CHAR_BIT * sizeof src ? 0 : getWordBit(src, positions - 1)
        );
      }

      updateFlagsZeroAndSign(result, state);
      updateFlagOverflow(src, result, state);
      advancePC(state);
    }
  };

  class And final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::AND;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto result = state.registers[decoded.reg2_addr].get<std::uint32_t>()
        & state.registers[decoded.reg3_addr].get<std::uint32_t>();

      state.registers[decoded.reg1_addr].set(result);

      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class AndImmediate final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::ANDI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto result = state.registers[decoded.reg2_addr].get<std::uint32_t>()
        & static_cast<std::uint32_t>(decoded.imm16);

      state.registers[decoded.reg1_addr].set(result);

      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class Or final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::OR;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto result = state.registers[decoded.reg2_addr].get<std::uint32_t>()
        | state.registers[decoded.reg3_addr].get<std::uint32_t>();

      state.registers[decoded.reg1_addr].set(result);

      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class OrImmediate final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::ORI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto result = state.registers[decoded.reg2_addr].get<std::uint32_t>()
        | static_cast<std::uint32_t>(decoded.imm16);

      state.registers[decoded.reg1_addr].set(result);

      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class Xor final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::XOR;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto result = state.registers[decoded.reg2_addr].get<std::uint32_t>()
        ^ state.registers[decoded.reg3_addr].get<std::uint32_t>();

      state.registers[decoded.reg1_addr].set(result);

      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class XorImmediate final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::XORI;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      const auto result = state.registers[decoded.reg2_addr].get<std::uint32_t>()
        ^ static_cast<std::uint32_t>(decoded.imm16);

      state.registers[decoded.reg1_addr].set(result);

      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  };

  class JumpRegister final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::JMPR;
    using DecodedType = ITypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      auto& pc = state.registers.getPC();
      
      const std::uint32_t return_address = pc.get<std::uint32_t>() + InstructionData::ENCODING_SIZE;
      const std::uint32_t target_address =
        state.registers[decoded.reg2_addr].get<std::uint32_t>()
        + static_cast<std::int32_t>(decoded.imm16)
      ;

      state.registers[decoded.reg1_addr].set(return_address); 
      pc.set(target_address);
    }
  };

  class JumpImmediate final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::JMPI;
    using DecodedType = JTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      auto& pc = state.registers.getPC();
      
      const std::uint32_t return_address = pc.get<std::uint32_t>() + InstructionData::ENCODING_SIZE;
      state.registers[decoded.reg1_addr].set(return_address); 
      
      const std::uint32_t target_address = 
        /* pc.get<std::uint32_t>() */
        + static_cast<std::int32_t>(decoded.imm20)
      ;
      // TODO: make relative
      pc.set(target_address);
    }
  };

  class Branch final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::B;
    using DecodedType = BTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      using enum Register::FlagIndex;

      const auto& decoded = std::get<DecodedType>(data.data);

      const auto& flags = state.registers.getFLAGS();
      auto& pc = state.registers.getPC();
      const std::uint32_t target = /* pc.get<std::uint32_t>() + */ decoded.imm20;
      // TODO: make relative

      switch (decoded.condition) {
        case ConditionType::BranchAlways:
          pc.set(target);
          return;
        case ConditionType::BranchNever:
          advancePC(state);
          return;
        case ConditionType::BranchIfEqual:
          if (flags.getBit(Zero)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchIfNotEqual:
          if (!flags.getBit(Zero)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchIfGreater:
          if (!flags.getBit(Zero) && flags.getBit(Sign) == flags.getBit(Overflow)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchIfLess:
          if (!flags.getBit(Zero) && flags.getBit(Sign) != flags.getBit(Overflow)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchIfGreaterEqual:
          if (flags.getBit(Zero) || flags.getBit(Sign) == flags.getBit(Overflow)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchIfLessEqual:
          if (flags.getBit(Zero) || flags.getBit(Sign) != flags.getBit(Overflow)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchUnsignedIfGreater:
          if (!flags.getBit(Zero) && !flags.getBit(Carry)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchUnsignedIfLess:
          if (flags.getBit(Carry)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchUnsignedIfGreaterEqual:
          if (!flags.getBit(Carry)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchUnsignedIfLessEqual:
          if (flags.getBit(Carry) || flags.getBit(Zero)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchCarry:
          if (flags.getBit(Carry)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchNotCarry:
          if (!flags.getBit(Carry)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchOverflow:
          if (flags.getBit(Overflow)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
        case ConditionType::BranchNotOverflow:
          if (!flags.getBit(Overflow)) {
            pc.set(target);
          } else {
            advancePC(state);
          }
          return;
      }
    }
  };

  class Halt final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::HALT;
    using DecodedType = RTypeData;

    static void execute([[maybe_unused]] const InstructionData::ParsedData& data, ProcessorState& state) {
      advancePC(state);
      throw exceptions::Halt {};
    }
  };

  class Cycle final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::CYCL;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);
      state.registers[decoded.reg1_addr].set(state.cycle_count);
      state.registers.getFLAGS().setBit(Register::FlagIndex::Zero, state.cycle_count == 0);
      advancePC(state);
    }
  };

  class Random final : public InstructionBase {
  public:
    constexpr static Opcode opcode = Opcode::RAND;
    using DecodedType = RTypeData;

    static void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
      const auto& decoded = std::get<DecodedType>(data.data);

      std::uniform_int_distribution<std::int32_t> distribution(
        state.registers[decoded.reg2_addr].get<std::int32_t>(),
        state.registers[decoded.reg3_addr].get<std::int32_t>()
      );

      const auto result = distribution(m_generator);
      state.registers[decoded.reg1_addr].set(result);

      updateFlagsZeroAndSign(result, state);
      advancePC(state);
    }
  
  private:
    inline static std::mt19937 m_generator { std::random_device {}() };
  };

  inline void execute(const InstructionData::ParsedData& data, ProcessorState& state) {
    try {
      switch (data.opcode) {
        #define EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE(mnemonic, class, ...) case Opcode::mnemonic: class::execute(data, state); break;
          EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST(EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE)
        #undef EMULATOR_CORE_INSTRUCTIONS_OPCODE_LIST_CASE
        default:
          throw exceptions::InvalidOperation{};
      }
      ++state.cycle_count;
    } catch (const exceptions::Halt&) {
      ++state.cycle_count;
      throw;
    }
  }
}

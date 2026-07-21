#pragma once

#include "Console.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <memory>
#include <span>

namespace emulator::core {
  class Memory {
  public:
    constexpr static size_t PAGE_SIZE{ 0x1000 };
    constexpr static size_t RAM_SIZE{ 1 << 24 };
    constexpr static size_t OFFSET_CONSOLE{ PAGE_SIZE };
    constexpr static size_t OFFSET_HEADER{ 0x10000 };
    constexpr static size_t OFFSET_CODE{ OFFSET_HEADER + PAGE_SIZE };
    
    constexpr static uint32_t CURRENT_VERSION{ 0 };

    struct Header {
      std::array<unsigned char, 8> tag{ 'e', 'm', 'u', 'l', 'a', 't', 'o', 'r' };
      uint32_t version{};
      uint32_t code_begin{ OFFSET_CODE };
      uint32_t data_begin{};
      uint32_t ram_begin{};
      uint32_t entry_point{};
    };

    constexpr static std::align_val_t MAX_ALIGNMENT{ std::max({
      alignof(Console),
      alignof(Header)
    })};

  public:
    Memory();
    Memory(const Memory& other);
    Memory(Memory&& other);
    Memory& operator=(const Memory& other);
    Memory& operator=(Memory&& other);
    ~Memory() = default;

    auto& getConsole(this auto&& self) {
      return *self.m_console;
    }

    auto& getHeader(this auto&& self) {
      return *self.m_header;
    }

    template <EmulatorType T>
    T get(size_t address) const {
      if (address < PAGE_SIZE) throw exceptions::InvalidReadAccess{};
      if (address + sizeof(T) > RAM_SIZE) throw exceptions::InvalidReadAccess{};

      T value{};
      std::memcpy(&value, m_memory.get() + address, sizeof(T));
      return value;
    }

    void set(size_t address, EmulatorType auto value) {
      if (address < PAGE_SIZE) throw exceptions::InvalidWriteAccess{};
      if (address + sizeof(value) > RAM_SIZE) throw exceptions::InvalidWriteAccess{};
      if (address + sizeof(value) > OFFSET_HEADER && address < getHeader().data_begin)
        throw exceptions::InvalidWriteAccess{};

      std::memcpy(m_memory.get() + address, &value, sizeof(value));
    }

    void loadProgram(std::span<const std::byte> binary);
    void loadProgram(const std::filesystem::path& file_path);

    bool operator==(const Memory& other) const;

  private:
    static bool validHeader(const Header& header);
    ptrdiff_t getComponentOffset(void* component) const;

  private:
    std::unique_ptr<std::byte[]> m_memory;
    Console* m_console;
    Header* m_header;
  };
}

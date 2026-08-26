#pragma once

#include "Console.hpp"
#include "Exceptions.hpp"
#include "KeyEvents.hpp"
#include "Types.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <span>

namespace core {
  class Memory {
  public:
    static constexpr uint32_t PAGE_SIZE { 0x1000 };
    static constexpr uint32_t RAM_SIZE { 1 << 24 };
    static constexpr uint32_t OFFSET_CONSOLE { PAGE_SIZE };
    static constexpr uint32_t OFFSET_KEY_MAPPINGS { 0x7000 };
    static constexpr uint32_t OFFSET_HEADER { 0x1'0000 };
    static constexpr uint32_t OFFSET_CODE { OFFSET_HEADER + PAGE_SIZE };

    static constexpr uint32_t CURRENT_VERSION { 0 };

    struct Header {
      std::array<unsigned char, 8> tag { 'e', 'm', 'u', 'l', 'a', 't', 'o', 'r' };
      uint32_t version {};
      uint32_t code_begin { OFFSET_CODE };
      uint32_t data_begin {};
      uint32_t ram_begin {};
      uint32_t entry_point {};
    };

    static constexpr std::align_val_t MAX_ALIGNMENT {
      std::max({ alignof(Console), alignof(Header), alignof(KeyEvents) })
    };

  public:
    Memory();
    Memory(const Memory& other);
    Memory(Memory&& other) noexcept;
    Memory& operator=(const Memory& other);
    Memory& operator=(Memory&& other) noexcept;
    ~Memory() = default;

    auto& getConsole(this auto&& self) {
      return *self.m_console;
    }

    auto& getHeader(this auto&& self) {
      return *self.m_header;
    }

    auto& getKeyEvents(this auto&& self) {
      return *self.m_key_events;
    }

    std::mutex& getMutex() const {
      return m_mutex;
    }

    template <EmulatorType T>
    T get(size_t address) const {
      if (address < PAGE_SIZE) {
        throw exceptions::InvalidReadAccess {};
      }
      if (address + sizeof(T) > RAM_SIZE) {
        throw exceptions::InvalidReadAccess {};
      }

      T value {};
      std::memcpy(&value, m_memory.get() + address, sizeof(T));
      return value;
    }

    void set(size_t address, EmulatorType auto value) {
      if (address < PAGE_SIZE) {
        throw exceptions::InvalidWriteAccess {};
      }
      if (address + sizeof(value) > RAM_SIZE) {
        throw exceptions::InvalidWriteAccess {};
      }
      if (address + sizeof(value) > OFFSET_HEADER && address < getHeader().data_begin) {
        throw exceptions::InvalidWriteAccess {};
      }

      std::memcpy(m_memory.get() + address, &value, sizeof(value));
    }

    std::span<const std::byte> getRaw() const {
      return { m_memory.get(), RAM_SIZE };
    }

    void loadProgram(std::span<const std::byte> binary);
    void loadProgram(const std::filesystem::path& file_path);

    bool operator==(const Memory& other) const;

  private:
    static std::byte* allocate();
    static bool validHeader(const Header& header);
    ptrdiff_t getComponentOffset(void* component) const;

  private:
    struct Deleter {
      void operator()(std::byte* ptr) const noexcept {
        ::operator delete[](ptr, MAX_ALIGNMENT);
      }
    };

  private:
    std::unique_ptr<std::byte[], Deleter> m_memory;
    Console* m_console;
    Header* m_header;
    KeyEvents* m_key_events;

    mutable std::mutex m_mutex {};
  };
}

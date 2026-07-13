#pragma once

#include "Console.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <span>
#include <utility>
#include <vector>

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
      alignof(Memory::Header)
    })};

  public:
    Memory():
      m_memory{ new (MAX_ALIGNMENT) std::byte[RAM_SIZE]{} },
      m_console{ std::start_lifetime_as<Console>(m_memory.get() + OFFSET_CONSOLE) },
      m_header{ std::start_lifetime_as<Header>(m_memory.get() + OFFSET_HEADER) } {
    }

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

    void loadProgram(std::span<const std::byte> binary) {
      if (binary.size() <= sizeof(Header)) throw exceptions::InvalidBinary{};
      if (binary.size() > RAM_SIZE - OFFSET_HEADER) throw exceptions::InvalidBinary{};

      Header new_header{};
      std::memcpy(&new_header, binary.data(), sizeof(Header));
      if (!validHeader(new_header)) throw exceptions::InvalidBinary{};

      std::memcpy(m_memory.get() + OFFSET_HEADER, binary.data(), binary.size_bytes());
    }

    void loadProgram(const std::filesystem::path& file_path) {
      std::ifstream file(file_path, std::ios_base::binary);

      const std::vector<char> binary{ std::istreambuf_iterator<char>{ file },
        std::istreambuf_iterator<char>{} };

      loadProgram({ reinterpret_cast<const std::byte*>(binary.data()), binary.size() });
    }

    bool validHeader(const Header& header) {
      if (header.tag != Header{}.tag) return false;
      if (header.code_begin != Header{}.code_begin) return false;
      if (header.version > CURRENT_VERSION) return false;
      if (header.data_begin < header.code_begin + PAGE_SIZE) return false;
      if (header.ram_begin < header.data_begin + PAGE_SIZE) return false;
      if (header.entry_point < header.code_begin ||
          header.entry_point >= header.data_begin) return false;

      if (header.code_begin % PAGE_SIZE != 0) return false;
      if (header.data_begin % PAGE_SIZE != 0) return false;
      if (header.ram_begin % PAGE_SIZE != 0) return false;

      return true;
    }

  private:
    std::unique_ptr<std::byte[]> m_memory;
    Console* m_console;
    Header* m_header;
  };
}

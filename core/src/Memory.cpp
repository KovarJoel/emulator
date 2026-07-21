#include "Memory.hpp"

#include <fstream>
#include <iterator>
#include <vector>

namespace emulator::core {
  Memory::Memory()
    : m_memory{ new (MAX_ALIGNMENT) std::byte[RAM_SIZE]{} },
      m_console{ std::start_lifetime_as<Console>(m_memory.get() + OFFSET_CONSOLE) },
      m_header{ std::start_lifetime_as<Header>(m_memory.get() + OFFSET_HEADER) } {
  }

  Memory::Memory(const Memory& other)
    : m_memory{ new (MAX_ALIGNMENT) std::byte[RAM_SIZE]{} },
      m_console{ std::start_lifetime_as<Console>(m_memory.get() + OFFSET_CONSOLE) },
      m_header{ std::start_lifetime_as<Header>(m_memory.get() + OFFSET_HEADER) } {
    std::memcpy(m_memory.get(), other.m_memory.get(), RAM_SIZE);
  }

  Memory::Memory(Memory&& other)
    : m_memory{ std::move(other.m_memory) },
      m_console{ other.m_console },
      m_header{ other.m_header } {
    other.m_console = nullptr;
    other.m_header = nullptr;
  }

  Memory& Memory::operator=(const Memory& other) {
    std::memcpy(m_memory.get(), other.m_memory.get(), RAM_SIZE);
    return *this;
  }

  Memory& Memory::operator=(Memory&& other) {
    m_memory = std::move(other.m_memory);
    m_console = other.m_console;
    m_header = other.m_header;
    other.m_console = nullptr;
    other.m_header = nullptr;
    return *this;
  }

  void Memory::loadProgram(std::span<const std::byte> binary) {
    if (binary.size() <= sizeof(Header)) throw exceptions::InvalidBinary{};
    if (binary.size() > RAM_SIZE - OFFSET_HEADER) throw exceptions::InvalidBinary{};

    Header new_header{};
    std::memcpy(&new_header, binary.data(), sizeof(Header));
    if (!validHeader(new_header)) throw exceptions::InvalidBinary{};

    std::memcpy(m_memory.get() + OFFSET_HEADER, binary.data(), binary.size_bytes());
  }

  void Memory::loadProgram(const std::filesystem::path& file_path) {
    std::ifstream file(file_path, std::ios_base::binary);

    const std::vector<char> binary{ std::istreambuf_iterator<char>{ file },
      std::istreambuf_iterator<char>{} };

    loadProgram({ reinterpret_cast<const std::byte*>(binary.data()), binary.size() });
  }

  bool Memory::operator==(const Memory& other) const {
    if (getComponentOffset(m_console) != other.getComponentOffset(other.m_console)) {
      return false;
    }
    if (getComponentOffset(m_header) != other.getComponentOffset(other.m_header)) {
      return false;
    }
    return std::memcmp(m_memory.get(), other.m_memory.get(), RAM_SIZE) == 0;
  }

  bool Memory::validHeader(const Header& header) {
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

  ptrdiff_t Memory::getComponentOffset(void* component) const {
    return std::distance(
      static_cast<const std::byte*>(m_memory.get()),
      static_cast<const std::byte*>(component)
    );
  }

}

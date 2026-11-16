#include "ElfFile.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <memory>
#include <stdexcept>

namespace common::Elf {
  void ElfFile::loadFromFile(const std::filesystem::path& file_path) {
    std::ifstream input_file { file_path, std::ios::in | std::ios::binary };

    if (!input_file) {
      throw std::runtime_error{ std::format("Invalid file path. Couldn't open {} for reading.", file_path.string()) };
    }

    input_file.seekg(0, std::ios::end);
    const auto input_file_size = input_file.tellg();

    input_file.seekg(0, std::ios::beg);
    std::vector<std::byte> input_file_contents(input_file_size);
    input_file.read(reinterpret_cast<char*>(input_file_contents.data()), input_file_size);

    loadFromMemory(input_file_contents);
  }
  
  void ElfFile::loadFromMemory(std::span<const std::byte> file_contents) {
    m_file_contents.resize(file_contents.size());
    std::copy(file_contents.begin(), file_contents.end(), m_file_contents.begin());

    loadHeaderStructures();
  }
  
  void ElfFile::writeToFile(const std::filesystem::path& file_path) {
    std::ofstream output_file { file_path, std::ios::out | std::ios::binary };

    if (!output_file) {
      throw std::runtime_error{ std::format("Invalid file path. Couldn't open {} for writing.", file_path.string()) };
    }

    output_file.write(reinterpret_cast<const char*>(m_file_contents.data()), m_file_contents.size());
  }

  const Defines::Elf32_Ehdr& ElfFile::getHeader() const {
    if (!m_elf_header) {
      throw std::runtime_error{ "Tried getting elf header from invalid file" };
    }
    return *m_elf_header;
  }

  bool ElfFile::hasValidHeader() const {
    using namespace Defines;
    using enum IdentIndex;

    const auto& header = getHeader();
    if (header.e_ident[std::to_underlying(EI_MAG0)]     != std::to_underlying(IdentMagicNumber::ELFMAG0))       return false;
    if (header.e_ident[std::to_underlying(EI_MAG1)]     != std::to_underlying(IdentMagicNumber::ELFMAG1))       return false;
    if (header.e_ident[std::to_underlying(EI_MAG2)]     != std::to_underlying(IdentMagicNumber::ELFMAG2))       return false;
    if (header.e_ident[std::to_underlying(EI_MAG3)]     != std::to_underlying(IdentMagicNumber::ELFMAG3))       return false;
    if (header.e_ident[std::to_underlying(EI_CLASS)]    != std::to_underlying(IdentClass::ELFCLASS32))          return false;
    if (header.e_ident[std::to_underlying(EI_DATA)]     != std::to_underlying(IdentDataEncoding::ELFDATA2LSB))  return false;
    if (header.e_ident[std::to_underlying(EI_VERSION)]  != std::to_underlying(VersionNumber::EV_CURRENT))       return false;
    if (header.e_ident[std::to_underlying(EI_OSABI)]    != std::to_underlying(IdentOsAbi::ELFOSABI_NONE))       return false;
    if (constexpr Elf32_Byte abi_version = 0; header.e_ident[std::to_underlying(EI_ABIVERSION)] != abi_version) return false;
    
    if (header.e_type     != ObjectFileType::ET_EXEC)    return false;
    if (header.e_machine  != Machine::EM_NONE)           return false;
    if (header.e_version  != VersionNumber::EV_CURRENT)  return false;

    if (header.e_phoff == 0) return false;

    return true;
  }

  void ElfFile::loadHeaderStructures() {
    if (m_file_contents.size() < sizeof *m_elf_header) {
      throw std::runtime_error{ "Unable to read elf header, the files contents are to small" };
    }
    m_elf_header = std::start_lifetime_as<Defines::Elf32_Ehdr>(m_file_contents.data());

    if (!hasValidHeader()) {
      throw std::runtime_error{ "Elf header does not conform to the expected data layout" };
    }

    if (m_elf_header->e_phentsize != sizeof *m_program_headers) {
      throw std::runtime_error{ "Mismatch in expected and received program header size" };
    }

    if (m_file_contents.size() < getHeader().e_phoff + m_elf_header->e_phnum * m_elf_header->e_phentsize) {
      throw std::runtime_error{ "Unable to read program headers, the files contents are to small" };
    }

    m_program_headers = std::start_lifetime_as_array<Defines::Elf32_Phdr>(m_file_contents.data() + getHeader().e_phoff, m_elf_header->e_phnum);
  }
}

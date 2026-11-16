#pragma once

#include <common/Elf/Defines.hpp>

#include <cstddef>
#include <filesystem>
#include <span>
#include <vector>

namespace common::Elf {
  class ElfFile {
  public:
    ElfFile() = default;
    
    ElfFile(std::span<const std::byte> file_contents) {
      loadFromMemory(file_contents);
    }

    ElfFile(const std::filesystem::path& file_path) {
      loadFromFile(file_path);
    }

    void loadFromFile(const std::filesystem::path& file_path);
    void loadFromMemory(std::span<const std::byte> file_contents);

    void writeToFile(const std::filesystem::path& file_path);
    
    inline const std::vector<std::byte>& getFileContents() const noexcept {
      return m_file_contents;
    }

    const Defines::Elf32_Ehdr& getHeader() const;
    bool hasValidHeader() const;

    inline std::span<const Defines::Elf32_Phdr> getProgramHeaders() const noexcept {
      if (!m_program_headers) return {};
      return { m_program_headers, m_elf_header->e_phnum };
    }

  private:
    void loadHeaderStructures();

  private:
    std::vector<std::byte> m_file_contents{};

    Defines::Elf32_Ehdr* m_elf_header{};
    Defines::Elf32_Phdr* m_program_headers{};
  };
}

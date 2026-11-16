#include "ElfFile.hpp"
#include <core/Memory.hpp>

#include <cstdio>
#include <cstdlib>
#include <format>
#include <fstream>
#include <print>

template <typename... Args>
[[noreturn]] inline static void makeError(std::format_string<Args...> format, Args&&... args) noexcept {
  std::print(stderr, "[ERROR] elf2bin: ");
  std::println(stderr, format, std::forward<Args>(args)...);
  exit(EXIT_FAILURE);
}

static void elf2Bin(const common::Elf::ElfFile& elf_file, const std::filesystem::path& output_path) {
  std::vector<std::byte> data_buffer{};

  std::size_t data_begin = 0;
  
  for (const auto& program_header : elf_file.getProgramHeaders()) {
    using enum common::Elf::Defines::SegmentType;
    using enum common::Elf::Defines::SegmentFlags;

    if (program_header.p_type != PT_LOAD) {
      makeError("Encountered unexpected segment type in program header ({})",
        std::to_underlying(program_header.p_type));
    }

    const std::size_t addr_begin = program_header.p_vaddr;
    const std::size_t addr_end = program_header.p_vaddr + program_header.p_memsz;

    if (!common::Elf::Defines::hasFlag(program_header.p_flags, PF_X)) {
      data_begin = std::min(data_begin, addr_begin);
    }

    if (addr_begin < core::Memory::OFFSET_CODE) {
      makeError("Can't allocate program memory before the code section ({}, valid begin at {})",
        addr_begin,
        core::Memory::OFFSET_CODE
      );
    }

    if (elf_file.getFileContents().size() < program_header.p_offset + program_header.p_filesz) {
      makeError("Program header points to non-existent offset in elf file ({}, size is {})",
        program_header.p_offset + program_header.p_filesz,
        elf_file.getFileContents().size()
      );
    }

    if (data_buffer.size() < addr_end) {
      data_buffer.resize(addr_end);
    }

    std::copy(
      elf_file.getFileContents().begin() + program_header.p_offset,
      elf_file.getFileContents().begin() + program_header.p_offset + program_header.p_filesz,
      data_buffer.begin()
    );
  }

  if (data_buffer.empty()) {
    makeError("Missing program data in elf file");
  }

  std::ofstream out_file { output_path, std::ios::out | std::ios::binary };
  if (!out_file) {
    makeError("File {} couldn't be opened for writing", output_path.string());
  }

  core::Memory::Header header{};
  header.data_begin = static_cast<common::Elf::Defines::Elf32_Word>(data_begin ? data_begin : core::Memory::OFFSET_CODE + core::Memory::PAGE_SIZE);
  header.ram_begin = static_cast<common::Elf::Defines::Elf32_Word>(core::Memory::OFFSET_CODE + data_buffer.size());
  header.entry_point = elf_file.getHeader().e_entry;

  memcpy(data_buffer.data(), &header, sizeof header);
  out_file.write(reinterpret_cast<const char*>(data_buffer.data()), data_buffer.size());
}

int main(int argc, const char** argv) {
  constexpr int expected_argc = 3;
  constexpr std::size_t input_file_path_index = 1;
  constexpr std::size_t output_file_path_index = 2;

  if (argc != expected_argc) {
    makeError("Invalid argument count, expected {}, got {}", expected_argc, argc);
  }

  common::Elf::ElfFile elf_file{ argv[input_file_path_index] };

  elf2Bin(elf_file, argv[output_file_path_index]);
}

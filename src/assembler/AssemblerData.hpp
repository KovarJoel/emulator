#pragma once

#include <string>
#include <vector>

namespace assembler {
  struct AssemblerData {
    std::string input_file_path {};
    std::string output_file_path {};

    std::vector<std::string> lines {};
  };
}

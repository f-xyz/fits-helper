#include "cli/hexdump.hpp"

namespace utils::cli {

std::string hexdump(std::span<const std::byte> data) {
  std::string result;

  for (std::size_t i = 0; i < data.size();) {
    std::size_t inc = std::min<std::size_t>(data.size() - i, 16);

    // Address offset
    result += std::format("{:04X} | ", i);

    // Hex data
    for (std::size_t j = i; j < i + 16; ++j) {
      if (j < i + inc) {
        result +=
            std::format("{:02X} ", std::to_integer<unsigned char>(data[j]));
        // 8 byte separator
        if (j == i + 7) {
          result += "| ";
        }
      } else {
        result += "   ";
        // 8 byte separator
        if (j == i + 7) {
          result += "| ";
        }
      }
    }

    result += "| ";

    // ASCII data
    for (std::size_t j = i; j < i + 16; ++j) {
      if (j < i + inc) {
        unsigned char c = std::to_integer<unsigned char>(data[j]);
        if (!std::iscntrl(c) && c >= 33 && c <= 126) {
          result += static_cast<char>(c);
        } else {
          result += '.';
        }
      }
    }

    result += "\n";
    i += inc;
  }

  return result;
}

} // namespace utils::cli
#include "cli/colors.hpp"

namespace utils::cli {

std::string rgb(const std::string &s, byte r, byte g, byte b) {
  const auto red = std::to_string(r);
  const auto green = std::to_string(g);
  const auto blue = std::to_string(b);

  return "\033[38;2;" + red + ";" + green + ";" + blue + "m" + s +
         constants::RESET;
}

std::string rgb(const std::string &s, unsigned int color) {
  unsigned char r = (color >> 16) & 0xFF;
  unsigned char g = (color >> 8) & 0xFF;
  unsigned char b = (color) & 0xFF;

  return rgb(s, r, g, b);
}

std::string bold(const std::string &s) {
  return constants::BOLD + s + constants::RESET;
}

std::string removeColors(const std::string &s) {
  const std::regex regex("\\x1B\\[[0-9;]*[a-zA-Z]");
  return std::regex_replace(s, regex, "");
}

} // namespace utils::cli
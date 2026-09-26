#include <astroutils/cli/colors.hpp>
#include <iterator>
#include <regex>

namespace astroutils::cli {

std::string rgb(std::string_view s, byte r, byte g, byte b) {
  const auto red = std::to_string(r);
  const auto green = std::to_string(g);
  const auto blue = std::to_string(b);

  std::string result = "\033[38;2;" + red + ";" + green + ";" + blue + "m";
  result.append(s);
  result += constants::RESET;
  return result;
}

std::string rgb(std::string_view s, unsigned int color) {
  unsigned char r = (color >> 16) & 0xFF;
  unsigned char g = (color >> 8) & 0xFF;
  unsigned char b = (color) & 0xFF;

  return rgb(s, r, g, b);
}

std::string bold(std::string_view s) {
  std::string result = constants::BOLD;
  result.append(s);
  result += constants::RESET;
  return result;
}

std::string removeColors(std::string_view s) {
  const std::regex regex("\\x1B\\[[0-9;]*[a-zA-Z]");
  std::string result;
  std::regex_replace(std::back_inserter(result), s.begin(), s.end(), regex, "");
  return result;
}

} // namespace astroutils::cli

#include "string.hpp"
#include "cli/colors.hpp"
#include <codecvt>
#include <cstddef>
#include <ranges>
#include <regex>

namespace utils::string {

std::vector<std::string> split(const std::string &str,
                               const std::string &delimiter) {
  auto parts = std::views::split(str, delimiter);
  return std::ranges::to<std::vector<std::string>>(parts);
}

std::string join(const std::vector<std::string> &arr,
                 const std::string &delimiter) {
  auto joined = std::views::join_with(arr, delimiter);
  return std::ranges::to<std::string>(joined);
}

////////////////////////////////////////
// Regex ///////////////////////////////
////////////////////////////////////////

std::optional<std::string> match(const std::regex &regex,
                                 const std::string &str, std::size_t nMatch) {
  std::smatch matches;
  std::regex_search(str, matches, regex);

  return matches.size() > nMatch
             ? std::optional<std::string>(matches[nMatch].str())
             : std::nullopt;
}

std::optional<std::string> match(const std::string &regex,
                                 const std::string &str, std::size_t nMatch) {
  return utils::string::match(std::regex(regex), str, nMatch);
}

std::string replace(const std::regex &regex, const std::string &string,
                    const std::string &replacement) {
  return std::regex_replace(string, regex, replacement);
}

std::string replace(const std::string &regex, const std::string &str,
                    const std::string &replacement) {
  std::regex rx(regex);
  return std::regex_replace(str, rx, replacement);
}

////////////////////////////////////////
// Slicing /////////////////////////////
////////////////////////////////////////

std::string slice(const std::string &str, int start) {
  start = start >= 0 ? start : str.size() + start;
  return {str.begin() + start, str.end()};
}

std::string slice(const std::string &str, int start, int end) {
  start = start >= 0 ? start : str.size() + start;
  end = end >= 0 ? end : str.size() + end;
  return {str.begin() + start, str.begin() + end};
}

////////////////////////////////////////
// Trimmming ///////////////////////////
////////////////////////////////////////

std::string ltrim(const std::string &str) {
  const auto pos = str.find_first_not_of(" \t\r\n");
  return str.substr(std::min(pos, str.size()));
}

std::string rtrim(const std::string &str) {
  const auto pos = str.find_last_not_of(" \t\r\n");
  return str.substr(0, pos + 1);
}

std::string trim(const std::string &str) { return ltrim(rtrim(str)); }

////////////////////////////////////////
// Case Conversion /////////////////////
////////////////////////////////////////

std::string uppercase(const std::string &str) {
  std::locale locale("C.UTF-8");
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring ws = converter.from_bytes(str);

  const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
  facet.toupper(&ws[0], &ws[0] + ws.size());

  return converter.to_bytes(ws);
}

std::string lowercase(const std::string &str) {
  std::locale locale("C.UTF-8");
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring ws = converter.from_bytes(str);

  const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
  facet.tolower(&ws[0], &ws[0] + ws.size());

  return converter.to_bytes(ws);
}

////////////////////////////////////////
// Formatting //////////////////////////
////////////////////////////////////////

std::string formatNumber(double x, int precision) {
  const auto str = std::format("{:.{}f}", x, precision);
  const auto getColor = [](double x) {
    return x > 0   ? 0x008000  // Green
           : x < 0 ? 0x800000  // Red
                   : 0x888888; // Gray
  };
  return std::format("{}", utils::cli::rgb(str, getColor(x)));
}

}; // namespace utils::string
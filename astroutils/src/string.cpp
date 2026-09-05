#include "string.hpp"
#include "cli/colors.hpp"
#include <codecvt>
#include <cstddef>
#include <ranges>
#include <regex>

namespace utils::string {

////////////////////////////////////////
// Splitting and Joining ///////////////
////////////////////////////////////////

std::vector<std::string> split(const std::string &string,
                               const std::string &delimiter) {
  auto parts = std::views::split(string, delimiter);
  return std::ranges::to<std::vector<std::string>>(parts);
}

std::string join(const std::vector<std::string> &array,
                 const std::string &delimiter) {
  auto joined = std::views::join_with(array, delimiter);
  return std::ranges::to<std::string>(joined);
}

////////////////////////////////////////
// Regex ///////////////////////////////
////////////////////////////////////////

std::optional<std::string> regex(const std::regex &regex,
                                 const std::string &string,
                                 const std::size_t nMatch) {
  std::smatch matches;
  std::regex_search(string, matches, regex);

  return matches.size() > nMatch
             ? std::optional<std::string>(matches[nMatch].str())
             : std::nullopt;
}

std::optional<std::string> regex(const std::string &regex,
                                 const std::string &string,
                                 const std::size_t nMatch) {
  return utils::string::regex(std::regex(regex), string, nMatch);
}

std::string regex_replace(const std::regex &regex, const std::string &string,
                          const std::string &replacement) {
  return std::regex_replace(string, regex, replacement);
}

std::string regex_replace(const std::string &regex, const std::string &string,
                          const std::string &replacement) {
  std::regex rx(regex);
  return std::regex_replace(string, rx, replacement);
}

////////////////////////////////////////
// Replacing ///////////////////////////
////////////////////////////////////////

std::string replace_all(std::string string, const std::string &find,
                        const std::string &replacement) {
  if (find.empty()) {
    return string;
  }

  std::size_t pos = 0;
  while ((pos = string.find(find, pos)) != std::string::npos) {
    string.replace(pos, find.length(), replacement);
    pos += replacement.length();
  }

  return string;
}

////////////////////////////////////////
// Slicing /////////////////////////////
////////////////////////////////////////

std::string slice(const std::string &string, int start) {
  start = start >= 0 ? start : string.size() + start;
  return {string.begin() + start, string.end()};
}

std::string slice(const std::string &string, int start, int end) {
  start = start >= 0 ? start : string.size() + start;
  end = end >= 0 ? end : string.size() + end;
  return {string.begin() + start, string.begin() + end};
}

////////////////////////////////////////
// Trimmming ///////////////////////////
////////////////////////////////////////

std::string ltrim(const std::string &string) {
  const auto pos = string.find_first_not_of(" \t\r\n");
  return string.substr(std::min(pos, string.size()));
}

std::string rtrim(const std::string &string) {
  const auto pos = string.find_last_not_of(" \t\r\n");
  return string.substr(0, pos + 1);
}

std::string trim(const std::string &string) { return ltrim(rtrim(string)); }

////////////////////////////////////////
// Case Conversion /////////////////////
////////////////////////////////////////

std::string uppercase(const std::string &string) {
  std::locale locale("C.UTF-8");
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring ws = converter.from_bytes(string);

  const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
  facet.toupper(&ws[0], &ws[0] + ws.size());

  return converter.to_bytes(ws);
}

std::string lowercase(const std::string &string) {
  std::locale locale("C.UTF-8");
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring ws = converter.from_bytes(string);

  const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
  facet.tolower(&ws[0], &ws[0] + ws.size());

  return converter.to_bytes(ws);
}

////////////////////////////////////////
// Formatting //////////////////////////
////////////////////////////////////////

std::string formatNumber(double number, int precision) {
  const auto string = std::format("{:.{}f}", number, precision);
  const auto getColor = [](double x) {
    return x > 0   ? 0x008000  // Green
           : x < 0 ? 0x800000  // Red
                   : 0x888888; // Gray
  };
  return std::format("{}", utils::cli::rgb(string, getColor(number)));
}

}; // namespace utils::string
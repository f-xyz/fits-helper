#pragma once

#include "cli/colors.hpp"
#include <optional>
#include <ranges>
#include <regex>
#include <string>

namespace utils::string {

inline std::vector<std::string>
split(const std::string &str, const std::string &delimeter) {
  return str
    | std::views::split(delimeter)
    | std::ranges::to<std::vector<std::string>>();
}

inline std::string
join(const std::vector<std::string> &arr, const std::string &delimeter) {
  return arr
    | std::views::join_with(delimeter)
    | std::ranges::to<std::string>();
}

inline std::optional<std::string>
regex(const std::regex &regex, const std::string &str, int nMatch = 0) {
  std::smatch matches;
  std::regex_search(str, matches, regex);

  return matches.size() > nMatch
    ? std::optional<std::string>(matches[nMatch].str())
    : std::nullopt;
}

inline std::optional<std::string>
regex(const std::string &regex, const std::string &str, int nMatch = 0) {
  return utils::string::regex(std::regex(regex), str, nMatch);
}

// TODO: regex replace

inline std::string slice(const std::string &str, int start) {
  start = start >= 0 ? start : str.size() + start;
  return {str.begin() + start, str.end()};
}

inline std::string slice(const std::string &str, int start, int end) {
  start = start >= 0 ? start : str.size() + start;
  end = end >= 0 ? end : str.size() + end;
  return {str.begin() + start, str.begin() + end};
}

inline std::string ltrim(const std::string &str) {
  const auto pos = str.find_first_not_of(" \t\r\n");
  return str.substr(std::min(pos, str.size()));
}

inline std::string rtrim(const std::string &str) {
  const auto pos = str.find_last_not_of(" \t\r\n");
  return str.substr(0, pos + 1);
}

inline std::string trim(const std::string &str) {
  return ltrim(rtrim(str));
}

inline std::string uppercase(const std::string &str) {
  if (str.empty()) {
    return "";
  }

  std::locale locale("C.UTF-8");
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring ws = converter.from_bytes(str);

  const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
  facet.toupper(&ws[0], &ws[0] + ws.size());

  return converter.to_bytes(ws);
}

inline std::string lowercase(const std::string &str) {
  if (str.empty()) {
    return "";
  }

  std::locale locale("C.UTF-8");
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring ws = converter.from_bytes(str);

  const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
  facet.tolower(&ws[0], &ws[0] + ws.size());

  return converter.to_bytes(ws);
}

inline std::string formatNumber(double x, int precision = 2) {
  const auto str = std::format("{:.{}f}", x, precision);
  const auto getColor = [](double x) {
    return x > 0
      ? 0x008000
      : x < 0
        ? 0x800000
        : 0x888888;
  };
  return std::format("{}", utils::cli::rgb(str, getColor(x)));
}

}; // namespace utils::string

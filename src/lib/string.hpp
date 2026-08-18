#pragma once

#include "cli/colors.hpp"
#include <string>

namespace utils::string {
  constexpr std::string_view slice(const std::string &s, int start) {
    start = start >= 0 ? start : s.size() + start;
    return {s.begin() + start, s.end()};
  }

  constexpr std::string_view slice(const std::string &s, int start, int end) {
    start = start >= 0 ? start : s.size() + start;
    end = end >= 0 ? end : -s.size() + end;
    return {s.begin() + start, s.end() - end};
  }

  inline std::string uppercase(const std::string &str) {
    if (str.empty()) {
      return "";
    }

    std::locale locale("C.UTF-8");
    std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
    std::wstring ws = converter.from_bytes(str);

    const auto &facet = std::use_facet<std::ctype<wchar_t> >(locale);
    facet.toupper(&ws[0], &ws[0] + ws.size());

    return converter.to_bytes(ws);
  }

  inline std::string lowercase(const std::string &str) {
    if (str.empty()) {
      return "";
    }

    std::locale locale("C.UTF-8");
    std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
    std::wstring ws = converter.from_bytes(str);

    const auto &facet = std::use_facet<std::ctype<wchar_t> >(locale);
    facet.tolower(&ws[0], &ws[0] + ws.size());

    return converter.to_bytes(ws);
  }

  constexpr std::string ltrim(const std::string &str) {
    const auto pos = str.find_first_not_of(" \t\r\n");
    return str.substr(std::min(pos, str.size()));
  }

  constexpr std::string rtrim(const std::string &str) {
    const auto pos = str.find_last_not_of(" \t\r\n");
    return str.substr(0, pos + 1);
  }

  constexpr std::string trim(const std::string &str) {
    return ltrim(rtrim(str));
  }

  constexpr std::string trimAndRemoveColors(const std::string &str) {
    return trim(removeColors(str));
  }

  constexpr std::string formatNumber(double x) {
    const auto str = std::to_string(x);
    if (x > 0) {
      return std::format("{}", rgb(str, 0, 128, 0));
    } else if (x < 0) {
      return std::format("{}", rgb(str, 128, 0, 0));
    } else {
      return std::format("{}", str);
    }
  }
};

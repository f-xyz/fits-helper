#pragma once

#include "../concepts.hpp"
#include "../math.hpp"
#include "colors.hpp"
#include <string>
#include <vector>

namespace utils::cli {

template <concepts::ArithmeticRange R>
std::string spark(const R &seq) {
  static const std::vector<std::string> chars = {
    "▁",
    "▂",
    "▃",
    "▄",
    "▅",
    "▆",
    "▇",
    "█"
  };

  if (std::empty(seq)) {
    return "[empty]";
  }

  const auto nBins = chars.size();
  const auto range = std::ranges::minmax(seq);

  std::string result = "[";

  for (const auto &x : seq) {
    const auto norm = utils::math::normalize(x, range);
    const auto bin = std::floor(norm * (nBins - 1));

    const int r = 255;
    const int g = std::floor(255 * norm);
    const int b = std::floor(64 * norm);
    result += rgb(chars[bin], r, g, b);
  }

  return result + "]";
}

template <typename T> requires std::is_arithmetic_v<T>
std::string spark(std::initializer_list<T> seq) {
  return spark(std::span<const T> {seq});
}

}; // namespace utils::cli
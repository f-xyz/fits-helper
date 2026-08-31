#pragma once

#include "concepts.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <random>
#include <vector>

namespace utils::math {

template <typename Range, typename Proj>
using keyOf = std::indirect_result_t<Proj, std::ranges::iterator_t<Range>>;

template <typename Range>
using valueOf = std::ranges::range_value_t<Range>;

////////////////////////////////////////
// Descriptive Statistics //////////////
////////////////////////////////////////

template <concepts::ArithmeticRange Range>
double sum(const Range &range) {
  return std::ranges::fold_left(range, 0.0, std::plus {});
}

template <concepts::ArithmeticRange Range>
double mean(const Range &range) {
  return math::sum(range) / std::size(range);
}

template <concepts::ArithmeticRandomAccessRange Range>
double median(Range range) {
  std::ranges::sort(range);

  const auto n = std::size(range);
  const auto m = n / 2;

  return n % 2 == 1
    ? range[std::floor(m)]
    : (static_cast<double>(range[m - 1]) + range[m]) / 2;
}

template <concepts::ArithmeticRange Range>
double variance(const Range &range) {
  double result = 0.0;
  const auto n = std::size(range);
  const auto avg = mean(range);

  for (const auto &x : range) {
    result += std::pow(x - avg, 2);
  }

  return n > 0 ? result / n : 0;
}

template <concepts::ArithmeticRange Range>
double stddev(const Range &range) {
  return std::sqrt(variance(range));
}

////////////////////////////////////////
// Python-Like Slices //////////////////
////////////////////////////////////////

template <std::ranges::input_range Range>
auto slice(Range &&range, int start) {
  start = start >= 0 ? start : std::size(range) + start;
  return std::vector(range.begin() + start, range.end());
}

template <std::ranges::input_range Range>
auto slice(Range &&range, int start, int end) {
  start = start >= 0 ? start : std::size(range) + start;
  end = end >= 0 ? end : std::size(range) + end;
  return std::vector(range.begin() + start, range.begin() + end);
}

////////////////////////////////////////
// Normalization ///////////////////////
////////////////////////////////////////

template <typename T> requires std::is_arithmetic_v<T>
double normalize(double value, T min, T max) {
  return min == max ? 0.5 : (value - min) / (max - min);
}

template <typename T> requires std::is_arithmetic_v<T>
double normalize(double value, const std::ranges::minmax_result<T> &range) {
  return normalize(value, range.min, range.max);
}

template <typename T> requires std::is_arithmetic_v<T>
double normalize(double value, const std::pair<T, T> &pair) {
  return normalize(value, pair.first, pair.second);
}

template <concepts::ArithmeticRange Range>
auto normalize(const Range &range) {
  auto result = std::ranges::to<std::vector<double>>(range);
  auto minmax = std::ranges::minmax(result);

  for (auto &x : result) {
    x = normalize(static_cast<double>(x), minmax);
  }

  return result;
}

template <concepts::ArithmeticRange Range>
auto rescale(const Range &range) {
  auto result = std::ranges::to<std::vector<double>>(range);
  auto sum = math::sum(result);

  if (sum != 0) {
    for (auto &x : result) {
      x = static_cast<double>(x) / sum;
    }
  } else if (!std::empty(range)) { // Non-empty having a 0 sum
    std::ranges::fill(result, 1.0 / result.size());
  }

  return result;
}

////////////////////////////////////////
// Groupping ///////////////////////////
////////////////////////////////////////

template <std::ranges::input_range Range, typename Proj = std::identity>
auto countBy(const Range& range, Proj proj = {}) {
  using Key = keyOf<Range, Proj>;
  std::map<Key, int> histogram;

  for (const auto &x : range) {
    const auto key = std::invoke(proj, x);
    ++histogram[key];
  }

  return histogram;
}

template <std::ranges::input_range Range, typename Proj = std::identity>
auto groupBy(const Range &range, Proj proj = {}) {
  using Key = keyOf<Range, Proj>;
  using Value = std::ranges::range_value_t<Range>;
  std::map<Key, std::vector<Value>> groups;

  for (const auto &x : range) {
    const auto key = std::invoke(proj, x);
    groups[key].push_back(x);
  }

  return groups;
}

////////////////////////////////////////
// Random //////////////////////////////
////////////////////////////////////////

inline double random(double min = 0, double max = 1) {
  static thread_local std::random_device rd;
  static thread_local std::mt19937 gen(rd());
  std::uniform_real_distribution<double> doubles(min, max);

  return doubles(gen);
}

} // namespace utils::math
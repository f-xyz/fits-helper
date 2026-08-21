#pragma once

#include "concepts.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <map>
#include <random>
#include <ranges>
#include <utility>

namespace utils::math {

template <typename R, typename Proj>
using key = std::remove_cvref_t<
  std::invoke_result_t<
    Proj,
    std::ranges::range_reference_t<R>
  >
>;

////////////////////////////////////////
// Descriptive Statistics //////////////
////////////////////////////////////////

template <concepts::ArithmeticRange R>
double sum(R &&seq) {
  return std::ranges::fold_left(seq, 0, std::plus {});
}

template <concepts::ArithmeticRange R>
double mean(R &&seq) {
  return math::sum(seq) / std::size(seq);
}

template <concepts::ArithmeticRange R>
double median(R seq) {
  std::ranges::sort(seq);
  const auto n = std::size(seq);
  const auto m = n / 2;
  return n % 2 == 1 ? seq[m] : (seq[std::floor(m)] + seq[std::ceil(m)]) / 2;
}

template <concepts::ArithmeticRange R>
double variance(R &&seq) {
  double result = 0;
  const auto avg = mean(seq);
  for (const auto &x : seq) {
    result += std::pow(x - avg, 2);
  }
  return result;
}

template <concepts::ArithmeticRange R>
double stddev(R &&seq) { 
  return std::sqrt(variance(seq));
}

////////////////////////////////////////
// Python-Like Slices //////////////////
////////////////////////////////////////

template <std::ranges::input_range R>
R slice(R &&seq, int start) {
  start = start >= 0 ? start : std::size(seq) + start;
  return {seq.begin() + start, seq.end()};
}

template <std::ranges::input_range R>
R slice(R &&seq, int start, int end) {
  start = start >= 0 ? start : std::size(seq) + start;
  end = end >= 0 ? end : std::size(seq) + end;
  return {seq.begin() + start, seq.end()};
}

////////////////////////////////////////
// Normalization ///////////////////////
////////////////////////////////////////

template <typename T> requires std::is_arithmetic_v<T>
double normalize(double value, T sum) {
  return value / sum;
}

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

template <concepts::ArithmeticRange R>
auto normalize(R &&seq) {
  auto result = std::ranges::to<std::vector<double>>(seq);
  auto range = std::ranges::minmax(result);

  for (auto &x : result) {
    x = normalize(static_cast<double>(x), range);
  }

  return result;
}

template <concepts::ArithmeticRange R>
auto rescale(R &&seq) {
  auto result = std::ranges::to<std::vector<double>>(seq);
  auto sum = math::sum(result);

  if (sum != 0) {
    for (auto &x : result) {
      x = static_cast<double>(x) / sum;
    }
  } else {
    std::ranges::fill(result, 1.0 / result.size());
  }

  return result;
}

////////////////////////////////////////
// Groupping ///////////////////////////
////////////////////////////////////////

template <std::ranges::input_range R,
          typename V = std::ranges::range_value_t<R>,
          typename Proj = std::identity>
auto countBy(R &&seq, Proj proj = {}) {
  using Key = key<R, Proj>;
  std::map<Key, int> hist;

  for (auto &&x : std::forward<R>(seq)) {
    const auto key = std::invoke(proj, x);
    ++hist[key];
  }

  return hist;
}

template <std::ranges::input_range R,
          typename V = std::ranges::range_value_t<R>,
          typename Proj = std::identity>
auto groupBy(R &&seq, Proj proj = {}) {
  using Key = key<R, Proj>;
  std::map<Key, std::vector<V>> groups;

  for (auto &&x : std::forward<R>(seq)) {
    const auto key = std::invoke(proj, x);
    const auto value = std::forward<decltype(x)>(x);
    groups[key].push_back(value);
  }

  return groups;
}

////////////////////////////////////////
// Random //////////////////////////////
////////////////////////////////////////

inline double random(double min = 0, double max = 1) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> doubles(min, max);

  return doubles(gen);
}

}; // namespace utils::math
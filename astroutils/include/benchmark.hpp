#pragma once

#include "concepts.hpp"
#include <chrono>
#include <functional>
#include <utility>

namespace utils {

template <concepts::ChronoDuration Duration = std::chrono::milliseconds,
          typename F, typename... Args>
Duration benchmark(F &&fn, Args &&...args) {
  const auto start = std::chrono::steady_clock::now();

  std::invoke(fn, std::forward<Args>(args)...);

  const auto end = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<Duration>(end - start);
}

} // namespace utils

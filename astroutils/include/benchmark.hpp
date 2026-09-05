#pragma once

#include "concepts.hpp"

namespace utils {

template <utils::concepts::ChronoDuration D = std::chrono::milliseconds,
          typename F, typename... Args>
D benchmark(F &&fn, Args &&...args) {
  const auto start = std::chrono::steady_clock::now();

  std::invoke(fn, std::forward<Args>(args)...);

  const auto end = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<D>(end - start);
}

} // namespace utils

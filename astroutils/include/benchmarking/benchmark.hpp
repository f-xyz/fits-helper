#pragma once

#include "Timer.hpp"
#include <chrono>
#include <functional>
#include <utility>

namespace utils::benchmarking {

template <concepts::Duration Units = std::chrono::milliseconds, typename F,
          typename... Args>
  requires std::invocable<F, Args...>
Units benchmark(F &&fn, Args &&...args) {
  Timer<Units> timer;
  std::invoke(fn, std::forward<Args>(args)...);
  return timer.measure();
}

} // namespace utils::benchmarking

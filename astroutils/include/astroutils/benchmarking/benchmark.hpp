#pragma once

#include <astroutils/benchmarking/Timer.hpp>
#include <chrono>
#include <functional>
#include <utility>

namespace astroutils::benchmarking {

template <concepts::Duration Units = std::chrono::milliseconds, typename F,
          typename... Args>
  requires std::invocable<F, Args...>
Units benchmark(F &&fn, Args &&...args) {
  Timer<Units> timer;
  std::invoke(fn, std::forward<Args>(args)...);
  return timer.measure();
}

} // namespace astroutils::benchmarking

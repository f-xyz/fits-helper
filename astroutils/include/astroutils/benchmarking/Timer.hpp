#pragma once

#include <astroutils/concepts.hpp>
#include <chrono>

namespace astroutils::benchmarking {

template <concepts::Duration Units = std::chrono::milliseconds> class Timer {
  std::chrono::time_point<std::chrono::steady_clock> start;

public:
  Timer() { reset(); }

  auto reset() {
    start = std::chrono::steady_clock::now();
    return *this;
  }

  auto measure() {
    const auto delta = std::chrono::steady_clock::now() - start;
    return std::chrono::duration_cast<Units>(delta);
  }
};

} // namespace astroutils::benchmarking

#pragma once

#include <chrono>
#include <concepts>
#include <fstream>
#include <ranges>
#include <type_traits>

namespace utils::concepts {

template <typename T>
concept ArithmeticRange = std::ranges::input_range<T> &&
                          std::is_arithmetic_v<std::ranges::range_value_t<T>>;

template <typename T>
concept ArithmeticRandomAccessRange =
    std::ranges::input_range<T> &&
    std::is_arithmetic_v<std::ranges::range_value_t<T>> &&
    std::ranges::random_access_range<T>;

template <typename T>
concept FileStream = std::same_as<std::remove_cvref_t<T>, std::ifstream> ||
                     std::same_as<std::remove_cvref_t<T>, std::ofstream> ||
                     std::same_as<std::remove_cvref_t<T>, std::fstream>;

template <typename T>
concept ChronoDuration =
    requires {
      typename T::rep;
      typename T::period;
    } &&
    std::same_as<T, std::chrono::duration<typename T::rep, typename T::period>>;

} // namespace utils::concepts
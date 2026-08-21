#pragma once

namespace utils::concepts {

template <typename T>
concept ArithmeticRange = std::ranges::input_range<T> &&
                          std::is_arithmetic_v<std::ranges::range_value_t<T>>;

};
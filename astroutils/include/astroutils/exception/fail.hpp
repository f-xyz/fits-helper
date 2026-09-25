#pragma once

namespace utils::exception {

template <typename... Args>
[[noreturn]] void fail(std::string_view message, Args &&...args) {
  const auto formatArgs = std::make_format_args(args...);
  const auto error = std::vformat(message, formatArgs);
  throw std::runtime_error(error);
}

} // namespace utils::exception
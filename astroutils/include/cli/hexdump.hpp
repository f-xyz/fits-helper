#pragma once

#include <cstddef>
#include <span>
#include <string>

namespace utils::cli {

std::string hexdump(std::span<const std::byte> data);

template <typename T>
std::string hexdump(const T *data, const std::size_t size) {
  const std::byte *pointer = reinterpret_cast<const std::byte *>(data);
  std::span<const std::byte> span(pointer, size);
  return hexdump(span);
}

} // namespace utils::cli
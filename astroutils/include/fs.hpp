#pragma once

#include "concepts.hpp"
#include <filesystem>
#include <format>
#include <regex>
#include <system_error>
#include <vector>

namespace utils::fs {

namespace internal {

template <concepts::FileStream T>
void throwIfNotOpen(const T &file, const std::filesystem::path &path) {
  if (!file.is_open()) {
    auto message = std::format("Failed to open file: {}", path.string());
    auto code = std::make_error_code(std::errc::no_such_file_or_directory);
    throw std::filesystem::filesystem_error(message, code);
  }
}

} // namespace internal

inline std::vector<std::filesystem::path> readDir(const std::filesystem::path &dir) {
  if (!std::filesystem::exists(dir)) {
    return {};
  }

  std::vector<std::filesystem::path> result;
  std::filesystem::directory_iterator iterator(dir);

  for (const auto &file : iterator) {
    result.push_back(file);
  }

  std::ranges::sort(result);
  return result;
}

template <std::ranges::contiguous_range R,
          typename T = std::ranges::range_value_t<R>>
inline auto writeFile(const std::filesystem::path &path, const R &content) {
  std::ofstream file(path, std::ios::binary);
  internal::throwIfNotOpen(file, path);

  auto data = std::data(content);
  auto size = std::size(content) * sizeof(T);
  file.write(reinterpret_cast<const char *>(data), size);

  return file;
}

template <typename T = std::byte>
inline std::vector<T> readFile(const std::filesystem::path &path) {
  std::ifstream file(path, std::ios::binary);
  internal::throwIfNotOpen(file, path.string());

  std::size_t size = std::filesystem::file_size(path);
  std::vector<T> buffer(size / sizeof(T));
  file.read(reinterpret_cast<char *>(buffer.data()), size);

  return buffer;
}

template <std::ranges::range R = std::string>
inline std::string readText(const std::filesystem::path &path) {
  std::ifstream file(path, std::ios::binary);
  internal::throwIfNotOpen(file, path.string());

  std::ostringstream oss;
  oss << file.rdbuf();

  return oss.str();
}

inline std::string tmpFile(const std::string &dir) {
  std::vector<std::filesystem::path> existing = readDir(dir);
  std::vector<int> indexes;

  std::ranges::transform(
      existing, std::back_inserter(indexes), [](const std::string &file) {
        std::smatch matches;
        std::string name = std::filesystem::path(file).filename().string();
        std::regex regex = std::regex(R"((\d+).*?$)");
        std::regex_search(name, matches, regex);
        int index = !matches.empty() ? std::stoi(matches[0].str()) : 0;
        return index;
      });

  int maxIndex = indexes.empty() ? 0 : *std::ranges::max_element(indexes);
  int newIndex = maxIndex + 1;

  std::string out = dir.ends_with('/') ? dir : dir + '/';
  return std::format("{}{}", out, newIndex);
}

inline std::string tmpFile() {
  const auto dir = std::filesystem::temp_directory_path().string();
  return tmpFile(dir);
}

} // namespace utils::fs

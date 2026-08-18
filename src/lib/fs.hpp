#pragma once

#include <filesystem>

namespace utils::fs {
  inline std::vector<std::string> readDir(const std::filesystem::path &dir) {
    if (!std::filesystem::exists(dir)) {
      return {};
    }

    std::vector<std::string> result;
    std::filesystem::directory_iterator iterator(dir);
    for (const auto &file: iterator) {
      if (file.is_regular_file()) {
        result.push_back(file.path());
      }
    }

    std::ranges::sort(result);
    return result;
  }

  inline std::string getTmpFileName(const std::string &dir) {
    std::vector<std::string> existing = readDir(dir);
    std::vector<int> indexes;

    std::ranges::transform(existing, std::back_inserter(indexes),
                           [](const std::string &file) {
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
};

#pragma once

#include "../SharpnessAnalyzer.h"
#include "StackerConfig.h"
#include <Logger.hpp>
#include <fs.hpp>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

using namespace utils;
using namespace utils::logging;

class StackerApp : StackerConfig {
  Logger &logger;
  SharpnessAnalyzer &analyzer;

public:
  StackerApp(const StackerConfig &config, Logger &logger, SharpnessAnalyzer &analyzer)
      : StackerConfig(config), logger(logger), analyzer(analyzer) {}

  void flatten() const;
  void chop() const;
  void stack() const;

private:
  std::vector<std::filesystem::path> readFiles() const {
    auto files = fs::readDir(directory) | std::views::filter(isRegularFile);
    return {files.begin(), files.end()};
  }

  static bool isRegularFile(const std::filesystem::path &path) {
    return std::filesystem::is_regular_file(path);
  }

  static bool isDirectory(const std::filesystem::path &path) {
    return std::filesystem::is_directory(path);
  }

  static bool comparePaths(const std::filesystem::path &a,
                           const std::filesystem::path &b) {
    try {
      return std::stoi(a.filename()) < std::stoi(b.filename());
    } catch (const std::invalid_argument &) {
      return false;
    }
  }
};
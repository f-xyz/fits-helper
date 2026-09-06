#pragma once

#include "Logger.hpp"
#include "StackerConfig.h"

using namespace utils;
using namespace utils::logging;

class StackerApp : StackerConfig {
  Logger &logger;

public:
  StackerApp(StackerConfig &config, Logger &logger)
      : StackerConfig(config), logger(logger) {}

  void flatten();
  void chop();
  void stack();

private:
  static bool isRegularFile(const std::filesystem::path &path) {
    return std::filesystem::is_regular_file(path);
  }

  static bool isDirectory(const std::filesystem::path &path) {
    return std::filesystem::is_directory(path);
  }

  static bool comparePaths(const std::filesystem::path &a,
                           const std::filesystem::path &b) {
    return std::stoi(a.filename()) < std::stoi(b.filename());
  }
};
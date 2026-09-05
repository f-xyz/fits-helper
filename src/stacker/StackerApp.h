#pragma once

#include "../Config.h"
#include "Logger.hpp"

using namespace utils;
using namespace utils::logging;

class StackerApp : Config::StackerConfig {
  Logger &logger;

public:
  StackerApp(Config &config, Logger &logger)
      : Config::StackerConfig(config.stacker), logger(logger) {}

  void chop();
  void stack();
  void unchop();

private:
  std::string getStackerScript(const std::filesystem::path &chunkDir);
  std::string getShellScript(const std::filesystem::path &chunkDir);
  std::string getCalibration();

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
#pragma once

#include "../Config.h"
#include "Logger.hpp"

using namespace utils;
using namespace utils::logging;

class StackerApp : Config::StackerConfig {
  Logger &logger;

public:
  StackerApp(Config &config, Logger &logger) : logger(logger) {
    directory = config.stacker.directory;
    size = config.stacker.size;
    dark = config.stacker.dark;
    flat = config.stacker.flat;
    bias = config.stacker.bias;
  }

  void chop();
  void stack();
  void unchop();

private:
  std::string getStackerScript(const std::filesystem::path &chunkDir);
  std::string getCalibration();

  static bool isRegularFile(const std::filesystem::path &path) {
    return std::filesystem::is_regular_file(path);
  }

  static bool isDirectory(const std::filesystem::path &path) {
    return std::filesystem::is_directory(path);
  }
};
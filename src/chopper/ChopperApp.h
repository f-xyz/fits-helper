#pragma once

#include "../Config.h"
#include "Logger.hpp"

using namespace utils;
using namespace utils::logging;

class ChopperApp : Config::ChopperConfig {
  Logger &logger;

public:
  ChopperApp(Config &config, Logger &logger) : logger(logger) {
    files = config.chopper.files;
    size = config.chopper.size;
    dark = config.chopper.dark;
    flat = config.chopper.flat;
    bias = config.chopper.bias;
    unchopDir = config.chopper.unchopDir;
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
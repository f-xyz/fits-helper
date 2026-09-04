#pragma once

#include "../Config.h"
#include "Logger.hpp"

using namespace utils;
using namespace utils::logging;

class ChopperApp : Config::CommonConfig, Config::ChopperConfig {
  Logger &logger;

public:
  ChopperApp(Config &config, Logger &logger) : logger(logger) {
    files = config.common.files;
    size = config.chopper.size;
    dir = config.chopper.dir;
  }

  void chop();
  void unchop();

private:
  std::string getStackerScript(const std::filesystem::path &chunkDir);

  static bool isRegularFile(const std::filesystem::path &path) {
    return std::filesystem::is_regular_file(path);
  }

  static bool isDirectory(const std::filesystem::path &path) {
    return std::filesystem::is_directory(path);
  }
};
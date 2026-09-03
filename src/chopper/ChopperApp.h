#pragma once

#include "../Config.h"
#include "Logger.hpp"
#include "fs.hpp"
#include "string.hpp"

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

  static bool isRegularFile(const std::string &file) {
    return std::filesystem::is_regular_file(file);
  }

  std::string getChunkDir(const std::string &baseDir, std::size_t iChunk) {
    return baseDir + "/" + std::to_string(iChunk + 1);
  }

  std::string getBaseDir(const std::filesystem::path &path) {
    return path.parent_path().string();
  }

  std::string getStackerScript(const std::string &dir) {
    const std::string templatePath = "scripts/stacker.ssf";
    const std::string tpl = fs::readText(templatePath);
    const std::string script = string::replace(R"(\{\$.+\})", tpl, dir);
    logger.info("{}", script);

    return script;
  }
};
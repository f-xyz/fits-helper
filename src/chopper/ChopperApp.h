#pragma once

#include "../Config.h"
#include "Logger.hpp"
#include "cli/colors.hpp"
#include "fs.hpp"
#include "string.hpp"

using namespace utils;
using namespace utils::logging;

class ChopperApp : Config::CommonConfig, Config::ChopperConfig {
  Logger &logger;

public:
  explicit ChopperApp(Config &config, Logger &logger) : logger(logger) {
    files = config.common.files;
    size = config.chopper.size;
  }

  void chop() {
    auto view = files
      | std::views::filter(isRegularFile)
      | std::views::chunk(size);

    using Chunk = std::vector<std::vector<std::string>>;
    auto chunks = std::ranges::to<Chunk>(view);

    for (std::size_t iChunk = 0; iChunk < chunks.size(); ++iChunk) {
      const std::vector<std::string> chunk = chunks[iChunk];
      const std::string baseDir = getBaseDir(chunk.front());
      logger.header("Base directory: {}", cli::bold(baseDir));

      const std::string chunkDir = getChunkDir(baseDir, iChunk);
      std::filesystem::create_directory(chunkDir);
      logger.info("Created directory: {}", cli::bold(chunkDir));

      for (const std::string &file : chunk) {
        const std::filesystem::path path(file);
        const std::string source = path.string();
        const std::string destination = chunkDir + "/" + path.filename().string();
        logger.info("from: {}", source);
        logger.info("to:   {}", destination);
        std::filesystem::rename(file, destination);
      }

      logger.info("");
    }
  }

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
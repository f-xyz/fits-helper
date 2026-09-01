#pragma once

#include "../Config.h"
#include "Logger.hpp"
#include "fs.hpp"
#include "process.hpp"
#include "string.hpp"

using namespace utils;
using namespace utils::logging;

class Chopper : Config::CommonConfig, Config::ChopperConfig {
  Logger &logger;

public:
  explicit Chopper(Config &config, Logger &logger) : logger(logger) {
    files = config.common.files;
    size = config.chopper.size;
  }

  void chop() {
    const auto chunks = files | std::views::chunk(size);

    for (const auto &chunk : chunks) {
      std::println("------------");
      const std::string dir = getDir(chunk.front());
      std::println("{}", utils::cli::bold(dir));

      for (const auto &file : chunk) {
        std::println("{}", file);
      }

      std::println("");
    }

    auto q = utils::process::exec("ping -c 4 8.8.8.8");
    // [](auto data) { std::print("{}", data); });
    std::println("{}", q.output);
  }

  std::string getDir(const std::filesystem::path &path) {
    return path.parent_path().string();
  }

  std::string getScript(const std::string &dir) {
    const std::string templatePath = "scripts/stacker.ssf";
    const std::string tpl = fs::readText(templatePath);
    const std::string script = string::replace(R"(\{\$.+\})", tpl, dir);
    std::println("{}", script);

    return script;
  }
};
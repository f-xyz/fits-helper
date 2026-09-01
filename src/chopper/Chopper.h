#pragma once

#include "../Config.h"
#include "Logger.hpp"
#include "fs.hpp"
#include "process.hpp"
#include "string.hpp"

using namespace utils::logging;

class Chopper : Config::CommonConfig, Config::ChopperConfig {
  Logger &logger;

public:
  explicit Chopper(Config &config, Logger &logger) : logger(logger) {
    files = config.common.files;
    size = config.chopper.size;
  }

  void chop() {
    const std::string templatePath = "scripts/stacker.ssf";
    const std::string tpl = utils::fs::readText(templatePath);
    const std::string script =
        utils::string::replace(R"(\{\$.+\})", tpl, "###");
    std::println("{}", script);

    const auto chunks = files | std::views::chunk(size);

    for (const auto &chunk : chunks) {
      std::println("------------");
      const std::filesystem::path first = chunk.front();
      const std::string dir = first.parent_path().string();
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
};
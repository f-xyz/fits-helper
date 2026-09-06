#pragma once

#include "Logger.hpp"
#include "process.hpp"

using namespace utils;

struct ScriptResult {
  int code;
  std::filesystem::path integration;
  std::chrono::seconds seconds;
};

class ScriptRunner {
  logging::Logger &logger;
  std::filesystem::path dir;
  ScriptResult result;

public:
  explicit ScriptRunner(logging::Logger &logger,
                        const std::filesystem::path &dir)
      : logger(logger), dir(dir) {}

  ScriptResult execute() {
    using namespace std::chrono;
    const auto start = steady_clock::now();

    result.code = stack();
    result.integration = cleanup();

    const auto end = steady_clock::now();
    result.seconds = duration_cast<seconds>(end - start);
    logger.info("  Time: {} sec", result.seconds);

    return result;
  }

private:
  int stack() {
    logger.info("Stacking...");

    const auto command = getCommand();
    const auto result = process::exec(command);

    return result.code;
  }

  std::filesystem::path cleanup() {
    logger.info("Cleaning up...");

    const std::filesystem::path tmp = "tmp";
    const std::filesystem::path integration = "integration.fit";

    const std::filesystem::path src = dir / tmp / integration;
    const std::filesystem::path dst = dir / integration;

    std::filesystem::rename(src, dst);
    std::filesystem::remove_all(dir / "tmp");

    return dst;
  }

  std::string getCommand() {
    const std::string bash = "bash";
    const std::string stack = (dir / "stack.sh").string();
    return bash + " " + stack;
  }
};
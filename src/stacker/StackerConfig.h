#pragma once

#include "../Subcommand.h"
#include <CLI11.hpp>
#include <filesystem>
#include <functional>

class StackerConfig {
public:
  std::filesystem::path sirilScript = "scripts/stacker.ssf";
  std::filesystem::path shellScript = "scripts/stack.sh";
  std::filesystem::path directory;
  int chunkSize = 10;
  std::filesystem::path dark;
  std::filesystem::path flat;
  std::filesystem::path bias;

  void bindSubcommands(CLI::App &app,
                       const std::function<void(Subcommand)> &callback);

private:
  void bindCommon(CLI::App *scmd);
  void bindStack(CLI::App *scmd);

  static std::string removeTerminalSlash(const std::string &str);
};
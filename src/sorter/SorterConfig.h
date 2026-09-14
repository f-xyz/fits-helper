#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <string>

#include <CLI11.hpp>

#include "../Subcommand.h"

class SorterConfig {
public:
  using Callback = std::function<void(Subcommand)>;
  enum class Select : char { Better, Worse };

  std::filesystem::path directory;
  Select select = Select::Better;
  double percentile = 0.1;
  int roi = 2;
  std::filesystem::path outDir;

  void bindSubcommands(CLI::App &app, const Callback &callback);

private:
  const std::map<std::string, SorterConfig::Select> selectMap = {
      {"better", SorterConfig::Select::Better},
      {"best", SorterConfig::Select::Better},
      {"worse", SorterConfig::Select::Worse},
      {"worst", SorterConfig::Select::Worse}};

  void setup(CLI::App *scmd, bool isMove);
};
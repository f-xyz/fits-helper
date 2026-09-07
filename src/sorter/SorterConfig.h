#pragma once

#include "../Subcommand.h"

class SorterConfig {
public:
  enum class Select { Better, Worse };

  std::vector<std::filesystem::path> files;
  Select select = Select::Better;
  double percentile = 0.1;
  int roi = 2;
  std::filesystem::path destination;

  void bindSubcommands(CLI::App &app, const std::function<void(Subcommand)> &callback) {
    const auto analyze = app.add_subcommand("analyze");
    setup(analyze, false);
    analyze->callback([&callback]() { callback(Subcommand::SorterAnalyze); });

    const auto move = app.add_subcommand("move");
    setup(move, true);
    move->callback([&callback]() { callback(Subcommand::SorterSort); });
  }

private:
  const std::map<std::string, SorterConfig::Select> selectMap = {
      {"better", SorterConfig::Select::Better},
      {"best", SorterConfig::Select::Better},
      {"worse", SorterConfig::Select::Worse},
      {"worst", SorterConfig::Select::Worse}};

  void setup(CLI::App *scmd, bool isMove) {
    scmd->add_option("-f,--files", files)
        ->description("Source files.")
        ->required(true)
        ->check(CLI::ExistingFile);

    scmd->add_option("-s,--select", select)
        ->transform(CLI::CheckedTransformer(selectMap, CLI::ignore_case))
        ->description("Select best or worst based on the percentile argument.")
        ->required(true)
        ->capture_default_str();

    scmd->add_option("-p,--percentile", percentile)
        ->description("Percentile to clip above or below.")
        ->required(true)
        ->capture_default_str();

    scmd->add_option("-r,--roi", roi)
        ->description("Portion of the image center to calculate sharpness.")
        ->required(false)
        ->capture_default_str();

    scmd->add_option("-d,--destination", destination)
        ->description("Destination directory.")
        ->required(isMove);
  }
};
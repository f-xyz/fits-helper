#include "SorterConfig.h"

void SorterConfig::bindSubcommands(CLI::App &app, const Callback &callback) {
  const auto analyze = app.add_subcommand("analyze");
  setup(analyze, false);
  analyze->callback([&callback]() { callback(Subcommand::SorterAnalyze); });

  const auto move = app.add_subcommand("move");
  setup(move, true);
  move->callback([&callback]() { callback(Subcommand::SorterSort); });
}

void SorterConfig::setup(CLI::App *scmd, bool isMove) {
  scmd->add_option("-d,--directory", directory)
      ->description("Source directory.")
      ->required(true)
      ->check(CLI::ExistingDirectory);

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

  scmd->add_option("-o,--out", outDir)
      ->description("Output directory.")
      ->required(isMove);
};
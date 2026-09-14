#include "StretcherConfig.h"

void StretcherConfig::bindSubcommands(
    CLI::App &app, const std::function<void(Subcommand)> &callback) {

  const auto stretch = app.add_subcommand("stretch");
  setup(stretch);
  stretch->callback([&callback]() { callback(Subcommand::Stretch); });
}

void StretcherConfig::setup(CLI::App *scmd) {
  scmd->add_option("-f,--file", file)
      ->description("Source file.")
      ->required(true)
      ->check(CLI::ExistingFile);

  scmd->add_option("-t,--type", stretchTypes)
      ->transform(CLI::CheckedTransformer(stretchTypeMap, CLI::ignore_case))
      ->description("Stretch type: CLAHE, Asinh. Default: CLAHE.")
      ->required(false)
      ->capture_default_str();

  scmd->add_option("-l,--limit", claheClipLimit)
      ->description("CLAHE clip limit. Default: 10.")
      ->required(false)
      ->capture_default_str();

  scmd->add_option("-s,--tile-size", claheTileSize)
      ->description("CLAHE tile size: Default: 8.")
      ->required(false)
      ->capture_default_str();

  scmd->add_option("-a,--asinh-factor", asinhFactor)
      ->description("Asinh stretch factor. Default: 0.2.")
      ->required(false)
      ->capture_default_str();

  scmd->add_option("-d,--denoise", denoise)
      ->description("Denoise. Default: 0 (disabled).")
      ->required(false)
      ->capture_default_str();
}
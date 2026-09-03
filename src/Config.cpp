#include "Config.h"
#include "CLI11.hpp"

int Config::parse(const int argc, const char **argv, const std::function<void(Subcommand)> &callback) {
  CLI::App app;
  app.require_subcommand(true);

  auto analyze = app.add_subcommand("analyze");
  addSorterOptionSelect(analyze, true);
  addSorterOptionPercentile(analyze, true);
  addSorterOptionRoi(analyze, false);
  addCommonOptionFiles(analyze, true);
  addSorterOptionDestination(analyze, false);
  analyze->callback([&callback]() {
    callback(Subcommand::Analyze);
  });

  auto move = app.add_subcommand("move");
  addSorterOptionSelect(move, true);
  addSorterOptionPercentile(move, true);
  addSorterOptionRoi(move, false);
  addCommonOptionFiles(move, true);
  addSorterOptionDestination(move, true);
  move->callback([&callback]() {
    callback(Subcommand::Move);
  });

  auto stretch = app.add_subcommand("stretch");
  addCommonOptionFiles(stretch, true);
  addStretcherOptionStretchType(stretch, false);
  addStretcherOptionClaheClipLimit(stretch, false);
  addStretcherOptionClaheTileSize(stretch, false);
  addStretcherOptionAsinhFactor(stretch, false);
  addStretcherOptionDenoise(stretch, false);
  stretch->callback([&callback]() {
    callback(Subcommand::Stretch);
  });

  auto chop = app.add_subcommand("chop");
  addCommonOptionFiles(chop, true);
  addChopperOptionSize(chop, false);
  chop->callback([&callback]() {
    callback(Subcommand::Chop);
  });

  auto unchop = app.add_subcommand("unchop");
  addChopperOptionDir(unchop, false);
  unchop->callback([&callback]() {
    callback(Subcommand::Unchop);
  });

  try {
    app.parse(argc, argv);
    return 0;
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }
}

//////////////////////////////////////
// Common options ////////////////////
//////////////////////////////////////

void Config::addCommonOptionFiles(CLI::App *app, bool isRequired) {
  app->add_option("-f,--files", common.files)
     ->description("Source files.")
     ->required(isRequired)
     ->check(CLI::ExistingFile);
}

//////////////////////////////////////
// Sorter options ////////////////////
//////////////////////////////////////

void Config::addSorterOptionSelect(CLI::App *app, bool isRequired) {
  app->add_option("-s,--select", sorter.select)
     ->transform(CLI::CheckedTransformer(selectMap, CLI::ignore_case))
     ->description("Select best or worst based on the percentile argument.")
     ->required(isRequired)
     ->capture_default_str();
}

void Config::addSorterOptionPercentile(CLI::App *app, bool isRequired) {
  app->add_option("-p,--percentile", sorter.percentile)
     ->description("Percentile to clip above or below.")
     ->required(isRequired)
     ->capture_default_str();
}

void Config::addSorterOptionRoi(CLI::App *app, bool isRequired) {
  app->add_option("-r,--roi", sorter.roi)
     ->description("Portion of the image center to calculate sharpness.")
     ->required(isRequired)
     ->capture_default_str();
}

void Config::addSorterOptionDestination(CLI::App *app, bool isRequired) {
  app->add_option("-d,--destination", sorter.destination)
     ->description("Destination directory.")
     ->required(isRequired);
}

//////////////////////////////////////
// Stretch options ///////////////////
//////////////////////////////////////

void Config::addStretcherOptionStretchType(CLI::App *app, bool isRequired) {
  app->add_option("-t,--type", stretcher.stretchTypes)
     ->transform(CLI::CheckedTransformer(stretchTypeMap, CLI::ignore_case))
     ->description("Stretch type: CLAHE, Asinh. Default: CLAHE.")
     ->required(isRequired)
     ->capture_default_str();
}

void Config::addStretcherOptionClaheClipLimit(CLI::App *app, bool isRequired) {
  app->add_option("-l,--limit", stretcher.claheClipLimit)
     ->description("CLAHE clip limit. Default: 10.")
     ->required(isRequired)
     ->capture_default_str();
}

void Config::addStretcherOptionClaheTileSize(CLI::App *app, bool isRequired) {
  app->add_option("-s,--tile-size", stretcher.claheTileSize)
     ->description("CLAHE tile size: Default: 8.")
     ->required(isRequired)
     ->capture_default_str();
}

void Config::addStretcherOptionAsinhFactor(CLI::App *app, bool isRequired) {
  app->add_option("-a,--asinh-factor", stretcher.asinhFactor)
     ->description("Asinh stretch factor. Default: 0.2.")
     ->required(isRequired)
     ->capture_default_str();
}

void Config::addStretcherOptionDenoise(CLI::App *app, bool isRequired) {
  app->add_option("-d,--denoise", stretcher.denoise)
     ->description("Denoise. Default: 0 (disabled).")
     ->required(isRequired)
     ->capture_default_str();
}

//////////////////////////////////////
// Chopper options ///////////////////
//////////////////////////////////////

void Config::addChopperOptionSize(CLI::App *app, bool isRequired) {
  app->add_option("-s,--size", chopper.size)
     ->description("Chunk size.")
     ->required(isRequired)
     ->capture_default_str();
}

void Config::addChopperOptionDir(CLI::App *app, bool isRequired) {
  app->add_option("-d,--dir", chopper.dir)
     ->description("Working directory.")
     ->required(isRequired)
     ->check(CLI::ExistingDirectory);
}
#pragma once

#include "lib/image/ImageStretcher.hpp"

class Config {
public:
  enum class Subcommand { Analyze, Move, Stretch };
  enum class Select { Better, Worse };

  struct CommonConfig {
    std::string logFilePath = "fits-helper.log";
    std::vector<std::string> files; // Common
  };

  struct SorterConfig {
    Select select = Select::Better;
    double percentile = 0.1;
    int roi = 2;
    std::string destination;
  };

  struct StretcherConfig {
    ImageStretcherOptions::Type stretchType = ImageStretcherOptions::Type::CLAHE;
    double claheClipLimit = 10;
    int claheTileSize = 8;
    float asinhFactor = 0.2;
    int denoise = 0;
  };

  CommonConfig common;
  SorterConfig sorter;
  StretcherConfig stretcher;

  int parse(const int argc, const char **argv, const std::function<void(Subcommand)> &callback) {
    CLI::App app;
    app.require_subcommand(1);

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

    try {
      app.parse(argc, argv);
      return 0;
    } catch (const CLI::ParseError &e) {
      return app.exit(e);
    }
  }

private:
  const std::map<std::string, Select> selectMap = {
    {"better", Select::Better},
    {"best", Select::Better},
    {"worse", Select::Worse},
    {"worst", Select::Worse}
  };

  const std::map<std::string, ImageStretcherOptions::Type> stretchTypeMap = {
    {"clahe", ImageStretcherOptions::Type::CLAHE},
    {"asinh", ImageStretcherOptions::Type::Asinh}
  };

  //////////////////////////////////////
  // Common options ////////////////////
  //////////////////////////////////////

  void addCommonOptionFiles(CLI::App *app, bool isRequired) {
    app->add_option("-f,--files", common.files)
       ->description("Source files.")
       ->required(isRequired)
       ->check(CLI::ExistingFile);
  }

  //////////////////////////////////////
  // Sorter options ////////////////////
  //////////////////////////////////////

  void addSorterOptionSelect(CLI::App *app, bool isRequired) {
    app->add_option("-s,--select", sorter.select)
       ->transform(CLI::CheckedTransformer(selectMap, CLI::ignore_case))
       ->description("Select best or worst based on the percentile argument.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addSorterOptionPercentile(CLI::App *app, bool isRequired) {
    app->add_option("-p,--percentile", sorter.percentile)
       ->description("Percentile to clip above or below.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addSorterOptionRoi(CLI::App *app, bool isRequired) {
    app->add_option("-r,--roi", sorter.roi)
       ->description("Portion of the image center to calculate sharpness.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addSorterOptionDestination(CLI::App *app, bool isRequired) {
    app->add_option("-d,--destination", sorter.destination)
       ->description("Destination directory.")
       ->required(isRequired);
  }

  //////////////////////////////////////
  // Stretch options ///////////////////
  //////////////////////////////////////

  void addStretcherOptionStretchType(CLI::App *app, bool isRequired) {
    app->add_option("-t,--type", stretcher.stretchType)
       ->transform(CLI::CheckedTransformer(stretchTypeMap, CLI::ignore_case))
       ->description("Stretch type: CLAHE, Asinh. Default: CLAHE.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addStretcherOptionClaheClipLimit(CLI::App *app, bool isRequired) {
    app->add_option("-l,--limit", stretcher.claheClipLimit)
       ->description("CLAHE clip limit. Default: 10.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addStretcherOptionClaheTileSize(CLI::App *app, bool isRequired) {
    app->add_option("-s,--tile-size", stretcher.claheTileSize)
       ->description("CLAHE tile size: Default: 8.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addStretcherOptionAsinhFactor(CLI::App *app, bool isRequired) {
    app->add_option("-a,--asinh-factor", stretcher.asinhFactor)
       ->description("Asinh stretch factor. Default: 0.2.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addStretcherOptionDenoise(CLI::App *app, bool isRequired) {
    app->add_option("-d,--denoise", stretcher.denoise)
       ->description("Denoise. Default: 0 (disabled).")
       ->required(isRequired)
       ->capture_default_str();
  }
};
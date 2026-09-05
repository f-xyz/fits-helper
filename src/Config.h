#pragma once

#include "image/ImageStretcher.hpp"
#include <filesystem>

using namespace utils::image;

class Config {
public:
  enum class Subcommand { Analyze, Move, Stretch, Chop, Unchop };
  enum class Select { Better, Worse };

  struct LoggerConfig {
    std::string logFilePath = "fits-helper.log";
  };

  struct SorterConfig {
    std::vector<std::filesystem::path> files;
    Select select = Select::Better;
    double percentile = 0.1;
    int roi = 2;
    std::filesystem::path destination;
  };

  struct StretcherConfig {
    std::filesystem::path file;
    using Type = ImageStretcherOptions::Type;
    std::vector<Type> stretchTypes = {Type::CLAHE};
    double claheClipLimit = 10;
    int claheTileSize = 8;
    float asinhFactor = 0.2;
    int denoise = 0;
  };

  struct ChopperConfig {
    std::filesystem::path scriptTemplatePath = "scripts/stacker.ssf";
    std::vector<std::filesystem::path> files;
    int size = 10;
    std::filesystem::path dark;
    std::filesystem::path flat;
    std::filesystem::path bias;
    std::filesystem::path unchopDir;
  };

  LoggerConfig common;
  SorterConfig sorter;
  StretcherConfig stretcher;
  ChopperConfig chopper;

  int parse(const int argc, const char **argv, const std::function<void(Subcommand)> &callback);

private:
  const std::map<std::string, Select> selectMap = {
    {"better", Select::Better},
    {"best", Select::Better},
    {"worse", Select::Worse},
    {"worst", Select::Worse}
  };

  const std::map<std::string, ImageStretcherOptions::Type> stretchTypeMap = {
    {"clahe", ImageStretcherOptions::Type::CLAHE},
    {"asinh", ImageStretcherOptions::Type::Asinh},
    {"hist", ImageStretcherOptions::Type::Histogram},
    {"histogram", ImageStretcherOptions::Type::Histogram}
  };

  //////////////////////////////////////
  // Sorter options ////////////////////
  //////////////////////////////////////

  void addSorterOptionFiles(CLI::App *app, bool isRequired);
  void addSorterOptionSelect(CLI::App *app, bool isRequired);
  void addSorterOptionPercentile(CLI::App *app, bool isRequired);
  void addSorterOptionRoi(CLI::App *app, bool isRequired);
  void addSorterOptionDestination(CLI::App *app, bool isRequired);

  //////////////////////////////////////
  // Stretcher options /////////////////
  //////////////////////////////////////

  void addStretcherOptionFile(CLI::App *app, bool isRequired);
  void addStretcherOptionStretchType(CLI::App *app, bool isRequired);
  void addStretcherOptionClaheClipLimit(CLI::App *app, bool isRequired);
  void addStretcherOptionClaheTileSize(CLI::App *app, bool isRequired);
  void addStretcherOptionAsinhFactor(CLI::App *app, bool isRequired);
  void addStretcherOptionDenoise(CLI::App *app, bool isRequired);

  //////////////////////////////////////
  // Chopper options ///////////////////
  //////////////////////////////////////

  void addChopperOptionFiles(CLI::App *app, bool isRequired);
  void addChopperOptionSize(CLI::App *app, bool isRequired);
  void addChopperOptionUnchopDir(CLI::App *app, bool isRequired);
  void addChopperOptionDark(CLI::App *app, bool isRequired);
  void addChopperOptionFlat(CLI::App *app, bool isRequired);
  void addChopperOptionBias(CLI::App *app, bool isRequired);
};
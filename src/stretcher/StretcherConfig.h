#pragma once

#include "../Subcommand.h"
#include <CLI11.hpp>
#include <astroutils/image/ImageStretcher.hpp>
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <vector>

class StretcherConfig {
public:
  using Type = astroutils::image::ImageStretcherOptions::Type;
  std::filesystem::path file;
  std::vector<Type> stretchTypes = {Type::CLAHE};
  double claheClipLimit = 10;
  int claheTileSize = 8;
  float asinhFactor = 0.2;
  float denoise = 0;

  void bindSubcommands(CLI::App &app,
                       const std::function<void(Subcommand)> &callback);

private:
  const std::map<std::string, Type> stretchTypeMap = {
      {"clahe", Type::CLAHE},
      {"asinh", Type::Asinh},
      {"hist", Type::Histogram},
      {"histogram", Type::Histogram}};

  void setup(CLI::App *scmd);
};
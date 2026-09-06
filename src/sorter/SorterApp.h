#pragma once

#include "Logger.hpp"
#include "SorterConfig.h"
#include "image/SharpnessEstimator.hpp"

using utils::logging::Logger;;
using utils::image::SharpnessEstimator;

class SorterApp : SorterConfig {
  struct Item {
    std::filesystem::path file;
    double sharpness = 0;
  };

  Logger &logger;
  SharpnessEstimator &estimator;
  std::vector<Item> results;

public:
  explicit SorterApp(SorterConfig &config, Logger &logger, SharpnessEstimator &estimator)
    : logger(logger), estimator(estimator) {
    files = config.files;
    select = config.select;
    percentile = config.percentile;
    roi = config.roi;
    destination = config.destination;
  }

  void analyzeFiles();
  void processFiles(bool moveFiles);

private:
  void printSpark();
  void printReportLine(const Item &item, bool isClipped, double percentile);
};
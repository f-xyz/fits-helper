#pragma once

#include "../Config.h"
#include "Logger.hpp"
#include "image/SharpnessEstimator.hpp"

using namespace utils::logging;

class SorterApp : Config::SorterConfig {
  struct Item {
    std::filesystem::path file;
    double sharpness = 0;
  };

  Logger &logger;
  SharpnessEstimator &estimator;
  std::vector<Item> results;

public:
  explicit SorterApp(Config &config, Logger &logger, SharpnessEstimator &estimator)
    : logger(logger), estimator(estimator) {
    files = config.sorter.files;
    select = config.sorter.select;
    percentile = config.sorter.percentile;
    roi = config.sorter.roi;
    destination = config.sorter.destination;
  }

  static cv::Mat readImage(const std::string &file);

  void analyzeFiles();
  void processFiles(bool moveFiles);

private:
  void printSpark();
  void printReportLine(const Item &item, bool isClipped, double percentile);
};
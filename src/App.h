#pragma once

#include "Config.h"
#include "utils/Logger.h"
#include "utils/spark.h"
#include "math/SharpnessEstimator.h"

class App {
  struct Item {
    std::string file;
    double sharpness = 0;
  };

  Config &config;
  Logger &logger;
  SharpnessEstimator &estimator;
  std::vector<Item> results;

public:
  explicit App(Config &config, Logger &logger, SharpnessEstimator &estimator)
    : config(config), logger(logger), estimator(estimator) {}

  static cv::Mat readImage(const std::string &file);

  void readFilesAndComputeSharpnesses();
  void computePercentiles(bool shouldMoveFiles);

  void printSpark();
  void printReportLine(const Item &item, bool isClipped, double percentile);
};
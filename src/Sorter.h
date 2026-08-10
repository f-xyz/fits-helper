#pragma once

#include "Config.h"
#include "utils/Logger.h"
#include "utils/spark.h"
#include "math/SharpnessEstimator.h"
#include <sys/select.h>

class Sorter : Config::CommonConfig, Config::SorterConfig {
  struct Item {
    std::string file;
    double sharpness = 0;
  };

  Logger &logger;
  SharpnessEstimator &estimator;
  std::vector<Item> results;

public:
  explicit Sorter(Config &config, Logger &logger, SharpnessEstimator &estimator)
    : logger(logger), estimator(estimator) {
    files = config.common.files;
    select = config.sorter.select;
    percentile = config.sorter.percentile;
    roi = config.sorter.roi;
    destination = config.sorter.destination;
  }

  static cv::Mat readImage(const std::string &file);

  void readFilesAndComputeSharpnesses();
  void computePercentiles(bool shouldMoveFiles);
  void printSpark();

private:
  void printReportLine(const Item &item, bool isClipped, double percentile);
};
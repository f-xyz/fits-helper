#pragma once

#include <Logger.hpp>
#include <filesystem>
#include <image/SharpnessEstimator.hpp>
#include <opencv2/core.hpp>
#include <vector>

using utils::image::SharpnessEstimator;
using utils::logging::Logger;

struct FileSharpness {
  std::filesystem::path file;
  std::filesystem::file_time_type modifiedAt;
  double sharpness = 0;
};

class SharpnessAnalyzer {
  Logger &logger;
  SharpnessEstimator &estimator;

public:
  SharpnessAnalyzer(Logger &logger, SharpnessEstimator &estimator)
      : logger(logger), estimator(estimator) {};

  std::vector<FileSharpness>
  analyzeFiles(const std::vector<std::filesystem::path> &files,
               int divider = 2) const;

private:
  double getSharpness(const std::filesystem::path &file, int divider) const;
  cv::Mat getRoi(const cv::Mat &image, int divider) const;
  double getProgress(const std::size_t nFiles,
                     std::atomic<std::size_t> &index) const;

  void log(const std::filesystem::path &file, double sharpness,
           double percents) const;
};
#pragma once

#include "../fits/FitsReader.h"
#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

namespace utils::images {

inline cv::Mat read(const std::string &file) {
  std::string ext = std::filesystem::path(file).extension().string();
  return ext == ".fit" || ext == ".fits" ? FitsReader().read(file)
                                         : cv::imread(file);
}

inline std::pair<double, double> range(const cv::Mat &image) {
  double min, max;
  cv::minMaxLoc(image, &min, &max);
  return {min, max};
}

inline std::vector<int> histogram(const cv::Mat &image,
                                  const int histSize = 16) {
  cv::Mat normalized;
  cv::normalize(image, normalized, 0, 255, cv::NORM_MINMAX, CV_8U);

  cv::Mat lab;
  cv::cvtColor(normalized, lab, cv::COLOR_BGR2Lab);

  constexpr int channels[] = {0};
  constexpr float range[] = {0, 256};
  const float *ranges[] = {range};

  cv::Mat hist;
  cv::calcHist(&lab, 1, channels, cv::noArray(), hist, 1, &histSize, ranges);

  // Converts cv::Mat<float> -> std::vector<float>
  return hist;
}

inline std::string info(const cv::Mat &image) {
  auto type = cv::typeToString(image.type());
  auto minmax = range(image);

  return std::format("{} {}x{} [{}-{}]", type, image.cols, image.rows,
                     minmax.first, minmax.second);
}

}; // namespace utils::images

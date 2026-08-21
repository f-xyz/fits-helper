#pragma once

#include "../fits/FitsReader.h"
#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

namespace utils::image {

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

inline cv::Mat normalize(const cv::Mat &image) {
  auto [min, max] = range(image);

  double scale = 255.0 / (max - min);
  double shift = -min * scale;

  cv::Mat result;
  image.convertTo(result, CV_8U, scale, shift);

  return result;
}

inline std::vector<cv::Mat> split(const cv::Mat &image) {
  std::vector<cv::Mat> channels;
  cv::split(image, channels);
  return channels;
}

inline cv::Mat merge(const std::vector<cv::Mat> channels) {
  cv::Mat result;
  cv::merge(channels, result);
  return result;
}

inline cv::Mat lightness(const cv::Mat &image) {
  if (image.channels() == 3) {
    cv::Mat lab, lightness;
    cv::cvtColor(image, lab, cv::COLOR_BGR2Lab);
    cv::extractChannel(lab, lightness, 0);
    return lightness;
  } else {
    return image;
  }
}

inline std::vector<int> histogram(const cv::Mat &image, const int bins = 16) {
  const int channels[] = {0};
  const float range[] = {0, 256};
  const float *ranges[] = {range};

  cv::Mat hist;
  cv::calcHist(&image, 1, channels, cv::noArray(), hist, 1, &bins, ranges);

  return hist; // Converts cv::Mat<float> -> std::vector<float>
}

inline cv::Mat clamp(const cv::Mat &image, int min, int max) {
  cv::Mat result;
  cv::max(image, min, result);
  cv::min(result, max, result);
  cv::normalize(result, result, 0, 255,
    cv::NORM_MINMAX);

  return result;
}

inline cv::Mat clahe(const cv::Mat &image, double limit = 10.0, int grid = 8) {
  cv::Mat result;

  auto clahe = cv::createCLAHE(limit, cv::Size(grid, grid));
  clahe->apply(image, result);

  return result;
}

inline std::tuple<int, int> soft_range(const cv::Mat &image, int nTopBins = 10) {
  auto hist = histogram(image, 256);

  std::vector<std::pair<int, int>> pairs;
  for (int i = 0; i < hist.size(); ++i) {
    pairs.push_back({ i, hist[i] });
  }

  std::ranges::sort(pairs,
    std::greater {},
    &std::pair<int, int>::second);

  int min = 255;
  int max = 0;

  for (int i = 0; i < nTopBins; ++i) {
    int value = pairs[i].first;
    int count = pairs[i].second;

    if (max < value) max = value;
    if (min > value) min = value;
  }

  return {min, max};
}

inline cv::Mat roi(const cv::Mat &image, int div = 2) {
  return image({
    image.cols / 2 - image.cols / (div * 2),
    image.rows / 2 - image.rows / (div * 2),
    image.cols / div,
    image.rows / div
  });
}

inline std::string info(const cv::Mat &image) {
  auto type = cv::typeToString(image.type());
  auto minmax = range(image);

  return std::format("{} {}x{} [{}-{}]",
    type, image.cols, image.rows,
    minmax.first, minmax.second);
}

inline void show(const cv::Mat &image, const int delay = 0,
            const cv::Size size = cv::Size(640, 480)) {
  cv::Mat preview;
  cv::resize(image, preview, size);
  cv::imshow("Image", preview);
  cv::waitKey(delay);
}

}; // namespace utils::images

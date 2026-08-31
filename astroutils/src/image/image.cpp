#include "image/image.hpp"
#include "fits/FitsReader.h"
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace utils::image {

cv::Mat read(const std::string &file) {
  std::string ext = std::filesystem::path(file).extension().string();
  return ext == ".fit" || ext == ".fits" ? FitsReader().read(file)
                                         : cv::imread(file);
}

std::pair<double, double> range(const cv::Mat &image) {
  double min, max;
  cv::minMaxLoc(image, &min, &max);
  return {min, max};
}

cv::Mat normalize(const cv::Mat &image) {
  auto [min, max] = range(image);

  double scale = min < max ? 255.0 / (max - min) : 127.0;
  double shift = -min * scale;

  cv::Mat result;
  image.convertTo(result, CV_8U, scale, shift);

  return result;
}

std::vector<cv::Mat> split(const cv::Mat &image) {
  std::vector<cv::Mat> channels;
  cv::split(image, channels);
  return channels;
}

cv::Mat merge(const std::vector<cv::Mat> channels) {
  cv::Mat result;
  cv::merge(channels, result);
  return result;
}

cv::Mat lightness(const cv::Mat &image) {
  if (image.channels() == 3) {
    cv::Mat lab, lightness;
    cv::cvtColor(image, lab, cv::COLOR_BGR2Lab);
    cv::extractChannel(lab, lightness, 0);
    return lightness;
  } else {
    return image;
  }
}

std::vector<int> histogram(const cv::Mat &image, const int bins) {
  const int channels[] = {0};
  const float range[] = {0, 256};
  const float *ranges[] = {range};

  cv::Mat hist;
  cv::calcHist(&image, 1, channels, cv::noArray(), hist, 1, &bins, ranges);

  return hist; // Converts cv::Mat<float> -> std::vector<float>
}

cv::Mat clamp(const cv::Mat &image, int min, int max) {
  cv::Mat result;
  cv::max(image, min, result);
  cv::min(result, max, result);
  cv::normalize(result, result, 0, 255, cv::NORM_MINMAX);

  return result;
}

cv::Mat clahe(const cv::Mat &image, double limit, int grid) {
  cv::Mat result;

  auto clahe = cv::createCLAHE(limit, cv::Size(grid, grid));
  clahe->apply(image, result);

  return result;
}

std::tuple<int, int> soft_range(const cv::Mat &image, int nTopBins) {
  auto hist = histogram(image, 256);

  std::vector<std::pair<int, int>> pairs;
  for (std::size_t i = 0; i < hist.size(); ++i) {
    pairs.push_back({i, hist[i]});
  }

  std::ranges::sort(pairs, std::greater{}, &std::pair<int, int>::second);

  int min = 255;
  int max = 0;

  for (int i = 0; i < nTopBins; ++i) {
    int value = pairs[i].first;
    if (max < value)
      max = value;
    if (min > value)
      min = value;
  }

  return {min, max};
}

cv::Mat roi(const cv::Mat &image, int div) {
  return image({image.cols / 2 - image.cols / (div * 2),
                image.rows / 2 - image.rows / (div * 2), image.cols / div,
                image.rows / div});
}

std::string info(const cv::Mat &image) {
  auto type = cv::typeToString(image.type());
  auto minmax = range(image);

  return std::format("{} {}x{} [{}-{}]", type, image.cols, image.rows,
                     minmax.first, minmax.second);
}

void show(const cv::Mat &image, const int delay, const cv::Size size) {
  cv::Mat preview;
  cv::resize(image, preview, size);
  cv::imshow("Image", preview);
  cv::waitKey(delay);
}

} // namespace utils::image
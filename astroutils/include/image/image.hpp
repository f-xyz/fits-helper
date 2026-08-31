#pragma once

#include <opencv2/core/mat.hpp>

namespace utils::image {

cv::Mat read(const std::string &file);

std::pair<double, double> range(const cv::Mat &image);

cv::Mat normalize(const cv::Mat &image);

std::vector<cv::Mat> split(const cv::Mat &image);

cv::Mat merge(const std::vector<cv::Mat> channels);

cv::Mat lightness(const cv::Mat &image);

std::vector<int> histogram(const cv::Mat &image, const int bins = 16);

cv::Mat clamp(const cv::Mat &image, int min, int max);

cv::Mat clahe(const cv::Mat &image, double limit = 10.0, int grid = 8);

std::tuple<int, int> soft_range(const cv::Mat &image, int nTopBins = 10);

cv::Mat roi(const cv::Mat &image, int div = 2);

std::string info(const cv::Mat &image);

void show(const cv::Mat &image, const int delay = 0,
          const cv::Size size = cv::Size(1280, 960));

} // namespace utils::image

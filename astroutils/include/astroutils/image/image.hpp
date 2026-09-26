#pragma once

#include <opencv2/core.hpp>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace astroutils::image {

////////////////////////////////////////
// Utilities ///////////////////////////
////////////////////////////////////////

cv::Mat read(std::string_view file);
cv::Mat normalize(const cv::Mat &image);
cv::Mat debayer(const cv::Mat &image);
cv::Mat clamp(const cv::Mat &image, double min, double max);
cv::Mat roi(const cv::Mat &image, int div = 2);
void show(const cv::Mat &image, const int delay = 0,
          const cv::Size size = cv::Size(1280, 960));

////////////////////////////////////////
// Channels ////////////////////////////
////////////////////////////////////////

cv::Mat lightness(const cv::Mat &image);
std::vector<cv::Mat> split(const cv::Mat &image);
cv::Mat merge(const std::vector<cv::Mat> &channels);

////////////////////////////////////////
// Informational ///////////////////////
////////////////////////////////////////

std::string info(const cv::Mat &image);
std::pair<double, double> range(const cv::Mat &image);
std::pair<double, double> soft_range(const cv::Mat &image, int nTopBins = 10);
std::vector<int> histogram(const cv::Mat &image, const int bins = 16);

} // namespace astroutils::image

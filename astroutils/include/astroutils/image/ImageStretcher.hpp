#pragma once

#include <opencv2/core.hpp>
#include <vector>

namespace utils::image {

struct ImageStretcherOptions {
  enum class Type { CLAHE, Asinh, Histogram };

  std::vector<Type> types = {Type::CLAHE};
  double claheClipLimit = 10;
  int claheTileSize = 8;
  float asinhFactor = 10;
  int histogramTopBins = 10;
  bool scaleChroma = true;
  int denoiseH = 0;
};

class ImageStretcher {
  ImageStretcherOptions options;

public:
  explicit ImageStretcher(const ImageStretcherOptions &options = {}) : options(options) {}
  cv::Mat stretch(const cv::Mat &image);

private:
  static cv::Mat getNormalizedLab(const cv::Mat &image);
  static cv::Mat getLightness(const cv::Mat &lightness);
  static void stretchClahe(cv::Mat &image, double clipLimit = 50.0,
                           int tileSize = 8);
  static void stretchAsinh(cv::Mat &image, float factor = 100);
  static void scaleChroma(std::vector<cv::Mat> &channels,
                          const cv::Mat &lightness);
};

} // namespace utils::image
#pragma once

#include <cstdint>
#include <opencv2/core.hpp>
#include <vector>

namespace astroutils::image {

struct ImageStretcherOptions {
  enum class Type: std::uint8_t { CLAHE, Asinh, Histogram };

  std::vector<Type> types = {Type::CLAHE};
  double claheClipLimit = 10;
  int claheTileSize = 8;
  float asinhFactor = 10;
  int histogramTopBins = 10;
  bool scaleChroma = true;
  float denoiseH = 0;
};

class ImageStretcher {
  ImageStretcherOptions options;

public:
  explicit ImageStretcher(const ImageStretcherOptions &options = {})
      : options(options) {}

  cv::Mat stretch(const cv::Mat &image);

private:
  static cv::Mat getNormalizedLab(const cv::Mat &image);
  static cv::Mat getLightness(const cv::Mat &lightness);

  static cv::Mat stretchClahe(cv::Mat &image, double clipLimit, int tileSize);
  static cv::Mat stretchAsinh(cv::Mat &image, float factor);
  static void scaleChroma(std::vector<cv::Mat> &channels, const cv::Mat &lightness);
};

} // namespace astroutils::image
#pragma once

#include <opencv2/core/mat.hpp>

namespace utils::image {

struct ImageStretcherOptions {
  enum class Type { CLAHE, Asinh, Histogram };

  Type type = Type::CLAHE;
  double claheClipLimit = 10;
  int claheTileSize = 8;
  float asinhFactor = 10;
  int histogramTopBins = 10;
  int denoiseH = 0;
};

class ImageStretcher {
  cv::Mat image;

public:
  ImageStretcher(const cv::Mat &image) : image(image) {}
  cv::Mat stretch(const ImageStretcherOptions &options = {});

private:
  static std::vector<cv::Mat> split(const cv::Mat &image);
  static cv::Mat getLightness(const cv::Mat &lightness);
  static void stretchClahe(cv::Mat &image, double clipLimit = 50.0,
                           int tileSize = 8);
  static void stretchAsinh(cv::Mat &image, float factor = 100);
  static void scaleChroma(std::vector<cv::Mat> &channels,
                          const cv::Mat &lightness);
};

} // namespace utils::image
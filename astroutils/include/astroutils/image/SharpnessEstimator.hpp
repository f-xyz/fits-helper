#pragma once

#include <opencv2/core.hpp>

namespace astroutils::image {

class SharpnessEstimator {
public:
  virtual ~SharpnessEstimator() = default;
  virtual double getSharpness(const cv::Mat &image) const = 0;

  static cv::Mat getGrayscaleImage(const cv::Mat &image);
  static cv::Mat getBlurredImage(const cv::Mat &image, double sigma);
  static double getStdDev(const cv::Mat &image);
};

class SharpnessEstimatorGaussian final : public SharpnessEstimator {
  double sigmaNarrow = 1;
  double sigmaWide = 10;

public:
  SharpnessEstimatorGaussian(double sigmaNarrow = 1, double sigmaWide = 10)
      : sigmaNarrow(sigmaNarrow), sigmaWide(sigmaWide) {}

  double getSharpness(const cv::Mat &image) const override;
};

class SharpnessEstimatorLaplacian final : public SharpnessEstimator {
  double sigmaHigh = 0;

public:
  SharpnessEstimatorLaplacian() = default;
  SharpnessEstimatorLaplacian(double sigma) : sigmaHigh(sigma) {}

  double getSharpness(const cv::Mat &image) const override;
};

} // namespace astroutils::image
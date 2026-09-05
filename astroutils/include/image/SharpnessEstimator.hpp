#pragma once

namespace utils::image {

class SharpnessEstimator {
public:
  virtual ~SharpnessEstimator() = default;
  virtual double getSharpness(const cv::Mat &image) const = 0;

  static cv::Mat getGrayscaleImage(const cv::Mat &image);
  static cv::Mat getBlurredImage(const cv::Mat &image, double sigma);
  static double getStdDev(const cv::Mat &image);
};

class SharpnessEstimatorGaussian final : public SharpnessEstimator {
  double sigmaHigh = 1;
  double sigmaLow = 10;

public:
  SharpnessEstimatorGaussian() = default;
  SharpnessEstimatorGaussian(double sigmaNarrow, double sigmaWide)
      : sigmaHigh(sigmaNarrow), sigmaLow(sigmaWide) {}

  double getSharpness(const cv::Mat &image) const override;
};

class SharpnessEstimatorLaplacian final : public SharpnessEstimator {
  double sigmaHigh = 0;

public:
  SharpnessEstimatorLaplacian() = default;
  SharpnessEstimatorLaplacian(double sigma) : sigmaHigh(sigma) {}

  double getSharpness(const cv::Mat &image) const override;
};

} // namespace utils::image
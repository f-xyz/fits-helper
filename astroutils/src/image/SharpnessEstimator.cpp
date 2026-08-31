#include "image/SharpnessEstimator.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace utils::image {

cv::Mat SharpnessEstimator::getGrayscaleImage(const cv::Mat &image) {
  cv::Mat gray, result;

  // Make grayscale
  if (image.channels() == 3) {
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
  } else {
    gray = image;
  }

  // Convert to float
  gray.convertTo(result, CV_32F);

  return result;
}

cv::Mat SharpnessEstimator::getBlurredImage(const cv::Mat &image,
                                            double sigma) {
  if (sigma > 0.0) {
    cv::Mat result;
    cv::Size kernel(0, 0);
    cv::GaussianBlur(image, result, kernel, sigma, sigma);
    return result;
  } else {
    return image;
  }
}

double SharpnessEstimator::getStdDev(const cv::Mat &image) {
  cv::Scalar mean, stdDev;
  cv::meanStdDev(image, mean, stdDev);
  return stdDev[0];
}

double SharpnessEstimatorGaussian::getSharpness(const cv::Mat &image) const {
  cv::Mat gray = getGrayscaleImage(image);

  cv::Mat high = getBlurredImage(gray, sigmaHigh);
  cv::Mat low = getBlurredImage(gray, sigmaLow);

  cv::Mat difference;
  cv::subtract(high, low, difference);

  return getStdDev(difference);
}

double SharpnessEstimatorLaplacian::getSharpness(const cv::Mat &image) const {
  cv::Mat gray = getGrayscaleImage(image);
  cv::Mat blurred = getBlurredImage(gray, sigmaHigh);

  cv::Mat laplacian;
  cv::Laplacian(blurred, laplacian, CV_32F);

  return getStdDev(laplacian);
}

} // namespace utils::image
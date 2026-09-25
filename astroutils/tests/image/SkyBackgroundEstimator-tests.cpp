#include <gtest/gtest.h>
#include <astroutils/image/SkyBackgroundEstimator.hpp>
#include <opencv2/imgproc.hpp>

TEST(SkyBackgroundEstimatorTest, ConstantBackgroundEstimation) {
  // Create 100x100 image with constant background of 50.0 and a few outlier bright pixels
  cv::Mat image(100, 100, CV_32FC1, cv::Scalar(50.0f));

  // Add bright star peak
  image.at<float>(50, 50) = 1000.0f;
  image.at<float>(51, 50) = 800.0f;

  astroutils::stars::BackgroundOptions options;
  options.method = astroutils::stars::BackgroundMethod::Constant;
  options.sigmaThreshold = 3.0;

  astroutils::stars::SkyBackgroundEstimator estimator(options);
  auto result = estimator.removeBackground(image);

  EXPECT_NEAR(result.meanBackground, 50.0, 1.0);
  EXPECT_NEAR(result.subtracted.at<float>(50, 50), 950.0f, 2.0f);
  EXPECT_NEAR(result.subtracted.at<float>(10, 10), 0.0f, 0.1f);
}

TEST(SkyBackgroundEstimatorTest, GridInterpolationBackgroundEstimation) {
  // Create 128x128 image with gradient background from 10.0 to 90.0
  cv::Mat image(128, 128, CV_32FC1);
  for (int r = 0; r < 128; ++r) {
    for (int c = 0; c < 128; ++c) {
      image.at<float>(r, c) = 10.0f + (r / 128.0f) * 80.0f;
    }
  }

  astroutils::stars::BackgroundOptions options;
  options.method = astroutils::stars::BackgroundMethod::GridInterpolation;
  options.gridSize = 32;

  astroutils::stars::SkyBackgroundEstimator estimator(options);
  auto result = estimator.removeBackground(image);

  // Gradient background should be mostly subtracted out
  double minVal, maxVal;
  cv::minMaxLoc(result.subtracted, &minVal, &maxVal);

  EXPECT_NEAR(maxVal, 0.0, 15.0);
}

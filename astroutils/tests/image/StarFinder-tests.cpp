#include <gtest/gtest.h>
#include <astroutils/image/StarFinder.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>

TEST(StarFinderTest, EmptyImageReturnsEmpty) {
  astroutils::stars::StarFinder finder;
  cv::Mat emptyMat;
  auto stars = finder.findStars(emptyMat);
  EXPECT_TRUE(stars.empty());
}

TEST(StarFinderTest, DetectSingleSyntheticStar) {
  // Create synthetic black image 100x100
  cv::Mat image = cv::Mat::zeros(100, 100, CV_32FC1);

  // Draw Gaussian-like star at center (50, 50) with sigma = 2.0 -> FWHM ~ 4.71, eccentricity ~ 0.0
  float cx = 50.0f;
  float cy = 50.0f;
  float sigma = 2.0f;

  for (int y = 0; y < 100; ++y) {
    for (int x = 0; x < 100; ++x) {
      float dx = x - cx;
      float dy = y - cy;
      float val = 1000.0f * std::exp(-(dx * dx + dy * dy) / (2.0f * sigma * sigma));
      image.at<float>(y, x) = val;
    }
  }

  astroutils::stars::StarFinderOptions options;
  options.thresholdSigma = 2.0;
  options.minArea = 5;

  astroutils::stars::StarFinder finder(options);
  auto stars = finder.findStars(image);

  ASSERT_EQ(stars.size(), 1u);
  const auto &star = stars[0];

  EXPECT_NEAR(star.center.x, cx, 0.5f);
  EXPECT_NEAR(star.center.y, cy, 0.5f);
  EXPECT_NEAR(star.fwhm, 2.35482 * sigma, 0.5);
  EXPECT_NEAR(star.eccentricity, 0.0, 0.15);
}

TEST(StarFinderTest, DetectElongatedSyntheticStar) {
  // Create synthetic black image 100x100 with an elongated star
  cv::Mat image = cv::Mat::zeros(100, 100, CV_32FC1);

  float cx = 50.0f;
  float cy = 50.0f;
  float sigmaX = 4.0f;
  float sigmaY = 2.0f;

  for (int y = 0; y < 100; ++y) {
    for (int x = 0; x < 100; ++x) {
      float dx = x - cx;
      float dy = y - cy;
      float val = 1000.0f * std::exp(-(dx * dx / (2.0f * sigmaX * sigmaX) + dy * dy / (2.0f * sigmaY * sigmaY)));
      image.at<float>(y, x) = val;
    }
  }

  astroutils::stars::StarFinderOptions options;
  options.thresholdSigma = 2.0;
  options.minArea = 5;

  astroutils::stars::StarFinder finder(options);
  auto stars = finder.findStars(image);

  ASSERT_EQ(stars.size(), 1u);
  const auto &star = stars[0];

  // Expected eccentricity e = sqrt(1 - (2^2 / 4^2)) = sqrt(1 - 0.25) = sqrt(0.75) ≈ 0.866
  EXPECT_NEAR(star.eccentricity, 0.866, 0.1);
  EXPECT_GT(star.fwhmX, star.fwhmY);
}

#include "image/SharpnessEstimator.hpp"
#include <opencv2/imgproc.hpp>

using ::testing::DoubleNear;
using ::testing::Gt;
using ::testing::Lt;
using utils::image::SharpnessEstimator;
using utils::image::SharpnessEstimatorGaussian;
using utils::image::SharpnessEstimatorLaplacian;

////////////////////////////////////////
// Static Helpers //////////////////////
////////////////////////////////////////

TEST(SharpnessEstimator, getGrayscaleImage_ColorInputConvertsToFloat) {
  // arrange
  cv::Mat color(4, 4, CV_8UC3, cv::Scalar(0, 0, 255));
  // act
  const auto result = SharpnessEstimator::getGrayscaleImage(color);
  // assert
  EXPECT_EQ(result.channels(), 1);
  EXPECT_EQ(result.type(), CV_32F);
  EXPECT_EQ(result.rows, 4);
  EXPECT_EQ(result.cols, 4);
}

TEST(SharpnessEstimator, getGrayscaleImage_GrayscaleInputConvertsToFloat) {
  // arrange
  cv::Mat gray(4, 4, CV_8UC1, cv::Scalar(128));
  // act
  const auto result = SharpnessEstimator::getGrayscaleImage(gray);
  // assert
  EXPECT_EQ(result.channels(), 1);
  EXPECT_EQ(result.type(), CV_32F);
  EXPECT_FLOAT_EQ(result.at<float>(0, 0), 128.0f);
}

TEST(SharpnessEstimator, getBlurredImage_ZeroSigmaReturnsOriginal) {
  // arrange
  cv::Mat img(4, 4, CV_32F, cv::Scalar(42.0f));
  // act
  const auto result = SharpnessEstimator::getBlurredImage(img, 0.0);
  // assert — same data pointer / same values, no blur applied
  EXPECT_EQ(cv::countNonZero(result != img), 0);
}

TEST(SharpnessEstimator, getBlurredImage_PositiveSigmaAppliesBlur) {
  // arrange — uniform image: blur shouldn't change values
  cv::Mat img(8, 8, CV_32F, cv::Scalar(1.0f));
  // act
  const auto result = SharpnessEstimator::getBlurredImage(img, 1.5);
  // assert — shape preserved
  EXPECT_EQ(result.type(), CV_32F);
  EXPECT_EQ(result.rows, 8);
  EXPECT_EQ(result.cols, 8);
}

TEST(SharpnessEstimator, getStdDev_UniformImageReturnsZero) {
  // arrange
  cv::Mat img(4, 4, CV_32F, cv::Scalar(99.0f));
  // act
  const double stdDev = SharpnessEstimator::getStdDev(img);
  // assert
  EXPECT_THAT(stdDev, DoubleNear(0.0, 1e-5));
}

TEST(SharpnessEstimator, getStdDev_VaryingImageReturnsPositive) {
  // arrange: two alternating values → non-zero std dev
  cv::Mat img = (cv::Mat_<float>(1, 4) << 0.0f, 1.0f, 0.0f, 1.0f);
  // act
  const double stdDev = SharpnessEstimator::getStdDev(img);
  // assert
  EXPECT_THAT(stdDev, Gt(0.0));
}

////////////////////////////////////////
// SharpnessEstimatorGaussian //////////
////////////////////////////////////////

TEST(SharpnessEstimatorGaussian, SharpCheckerboardScoresHigherThanBlurred) {
  // arrange
  cv::Mat sharp(16, 16, CV_8UC1);
  for (int y = 0; y < 16; ++y) {
    for (int x = 0; x < 16; ++x) {
      sharp.at<uchar>(y, x) = ((y + x) % 2 == 0) ? 0 : 255;
    }
  }

  cv::Mat blurred;
  cv::GaussianBlur(sharp, blurred, cv::Size(7, 7), 3.0);

  // act
  SharpnessEstimatorGaussian estimator(1.0, 5.0);
  const double sharpScore = estimator.getSharpness(sharp);
  const double blurredScore = estimator.getSharpness(blurred);

  // assert
  EXPECT_GT(sharpScore, blurredScore);
}

TEST(SharpnessEstimatorGaussian, UniformImageReturnsNearZero) {
  // arrange
  cv::Mat img(8, 8, CV_8UC1, cv::Scalar(128));
  // act
  SharpnessEstimatorGaussian estimator;
  const double score = estimator.getSharpness(img);
  // assert
  EXPECT_THAT(score, DoubleNear(0.0, 1.0));
}

TEST(SharpnessEstimatorGaussian, ColorImageIsAccepted) {
  // arrange — color input should be converted to grayscale internally
  cv::Mat img(8, 8, CV_8UC3, cv::Scalar(100, 150, 200));
  // act & assert — must not throw
  SharpnessEstimatorGaussian estimator;
  EXPECT_NO_THROW(estimator.getSharpness(img));
}

TEST(SharpnessEstimatorGaussian, CustomSigmasConstructor) {
  // arrange
  cv::Mat img(8, 8, CV_8UC1, cv::Scalar(128));
  // act
  SharpnessEstimatorGaussian estimator(0.5, 8.0);
  const double score = estimator.getSharpness(img);
  // assert
  EXPECT_THAT(score, DoubleNear(0.0, 1.0));
}

////////////////////////////////////////
// SharpnessEstimatorLaplacian /////////
////////////////////////////////////////

TEST(SharpnessEstimatorLaplacian, SharpCheckerboardScoresHigherThanBlurred) {
  // arrange
  cv::Mat sharp(16, 16, CV_8UC1);
  for (int y = 0; y < 16; ++y) {
    for (int x = 0; x < 16; ++x) {
      sharp.at<uchar>(y, x) = ((y + x) % 2 == 0) ? 0 : 255;
    }
  }

  cv::Mat blurred;
  cv::GaussianBlur(sharp, blurred, cv::Size(7, 7), 3.0);

  // act
  SharpnessEstimatorLaplacian estimator;
  const double sharpScore = estimator.getSharpness(sharp);
  const double blurredScore = estimator.getSharpness(blurred);

  // assert
  EXPECT_GT(sharpScore, blurredScore);
}

TEST(SharpnessEstimatorLaplacian, UniformImageReturnsNearZero) {
  // arrange
  cv::Mat img(8, 8, CV_8UC1, cv::Scalar(128));
  // act
  SharpnessEstimatorLaplacian estimator;
  const double score = estimator.getSharpness(img);
  // assert
  EXPECT_THAT(score, DoubleNear(0.0, 1.0));
}

TEST(SharpnessEstimatorLaplacian, ColorImageIsAccepted) {
  // arrange
  cv::Mat img(8, 8, CV_8UC3, cv::Scalar(100, 150, 200));
  // act & assert
  SharpnessEstimatorLaplacian estimator;
  EXPECT_NO_THROW(estimator.getSharpness(img));
}

TEST(SharpnessEstimatorLaplacian, PreBlurWithSigmaReducesScore) {
  // arrange — positive sigma pre-blurs before Laplacian, softening edges
  cv::Mat sharp(16, 16, CV_8UC1);
  for (int y = 0; y < 16; ++y) {
    for (int x = 0; x < 16; ++x) {
      sharp.at<uchar>(y, x) = ((y + x) % 2 == 0) ? 0 : 255;
    }
  }

  // act
  SharpnessEstimatorLaplacian withSigma(2.0);
  SharpnessEstimatorLaplacian noSigma(0.0);

  // assert — pre-blurring should lower the apparent sharpness
  EXPECT_LT(withSigma.getSharpness(sharp), noSigma.getSharpness(sharp));
}

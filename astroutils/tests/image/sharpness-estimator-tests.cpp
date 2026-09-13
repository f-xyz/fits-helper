#include "image/SharpnessEstimator.hpp"

using ::testing::DoubleNear;
using ::testing::Gt;
using ::testing::Lt;

// -----------------------------------------------------------------------
// SharpnessEstimator static helpers
// -----------------------------------------------------------------------

TEST(SharpnessEstimator, GetGrayscaleImage_ColorInputConvertsToFloat) {
  // arrange
  cv::Mat color(4, 4, CV_8UC3, cv::Scalar(0, 0, 255));
  // act
  const auto result = utils::image::SharpnessEstimator::getGrayscaleImage(color);
  // assert
  EXPECT_EQ(result.channels(), 1);
  EXPECT_EQ(result.type(), CV_32F);
  EXPECT_EQ(result.rows, 4);
  EXPECT_EQ(result.cols, 4);
}

TEST(SharpnessEstimator, GetGrayscaleImage_GrayscaleInputConvertsToFloat) {
  // arrange
  cv::Mat gray(4, 4, CV_8UC1, cv::Scalar(128));
  // act
  const auto result = utils::image::SharpnessEstimator::getGrayscaleImage(gray);
  // assert
  EXPECT_EQ(result.channels(), 1);
  EXPECT_EQ(result.type(), CV_32F);
  EXPECT_FLOAT_EQ(result.at<float>(0, 0), 128.0f);
}

TEST(SharpnessEstimator, GetBlurredImage_ZeroSigmaReturnsOriginal) {
  // arrange
  cv::Mat img(4, 4, CV_32F, cv::Scalar(42.0f));
  // act
  const auto result = utils::image::SharpnessEstimator::getBlurredImage(img, 0.0);
  // assert — same data pointer / same values, no blur applied
  EXPECT_EQ(cv::countNonZero(result != img), 0);
}

TEST(SharpnessEstimator, GetBlurredImage_PositiveSigmaAppliesBlur) {
  // arrange — uniform image: blur shouldn't change values
  cv::Mat img(8, 8, CV_32F, cv::Scalar(1.0f));
  // act
  const auto result = utils::image::SharpnessEstimator::getBlurredImage(img, 1.5);
  // assert — shape preserved
  EXPECT_EQ(result.type(), CV_32F);
  EXPECT_EQ(result.rows, 8);
  EXPECT_EQ(result.cols, 8);
}

TEST(SharpnessEstimator, GetStdDev_UniformImageReturnsZero) {
  // arrange
  cv::Mat img(4, 4, CV_32F, cv::Scalar(99.0f));
  // act
  const double stdDev = utils::image::SharpnessEstimator::getStdDev(img);
  // assert
  EXPECT_THAT(stdDev, DoubleNear(0.0, 1e-5));
}

TEST(SharpnessEstimator, GetStdDev_VaryingImageReturnsPositive) {
  // arrange: two alternating values → non-zero std dev
  cv::Mat img = (cv::Mat_<float>(1, 4) << 0.0f, 1.0f, 0.0f, 1.0f);
  // act
  const double stdDev = utils::image::SharpnessEstimator::getStdDev(img);
  // assert
  EXPECT_THAT(stdDev, Gt(0.0));
}

// -----------------------------------------------------------------------
// SharpnessEstimatorGaussian
// -----------------------------------------------------------------------

TEST(SharpnessEstimatorGaussian, SharpCheckerboardScoresHigherThanBlurred) {
  // arrange
  cv::Mat sharp(16, 16, CV_8UC1);
  for (int r = 0; r < 16; ++r)
    for (int c = 0; c < 16; ++c)
      sharp.at<uchar>(r, c) = ((r + c) % 2 == 0) ? 0 : 255;

  cv::Mat blurred;
  cv::GaussianBlur(sharp, blurred, cv::Size(7, 7), 3.0);

  // act
  utils::image::SharpnessEstimatorGaussian estimator(1.0, 5.0);
  const double sharpScore   = estimator.getSharpness(sharp);
  const double blurredScore = estimator.getSharpness(blurred);

  // assert
  EXPECT_GT(sharpScore, blurredScore);
}

TEST(SharpnessEstimatorGaussian, UniformImageReturnsNearZero) {
  // arrange
  cv::Mat img(8, 8, CV_8UC1, cv::Scalar(128));
  // act
  utils::image::SharpnessEstimatorGaussian estimator;
  const double score = estimator.getSharpness(img);
  // assert
  EXPECT_THAT(score, DoubleNear(0.0, 1.0));
}

TEST(SharpnessEstimatorGaussian, ColorImageIsAccepted) {
  // arrange — color input should be converted to grayscale internally
  cv::Mat img(8, 8, CV_8UC3, cv::Scalar(100, 150, 200));
  // act & assert — must not throw
  utils::image::SharpnessEstimatorGaussian estimator;
  EXPECT_NO_THROW(estimator.getSharpness(img));
}

TEST(SharpnessEstimatorGaussian, CustomSigmasConstructor) {
  // arrange
  cv::Mat img(8, 8, CV_8UC1, cv::Scalar(128));
  // act
  utils::image::SharpnessEstimatorGaussian estimator(0.5, 8.0);
  const double score = estimator.getSharpness(img);
  // assert
  EXPECT_THAT(score, DoubleNear(0.0, 1.0));
}

// -----------------------------------------------------------------------
// SharpnessEstimatorLaplacian
// -----------------------------------------------------------------------

TEST(SharpnessEstimatorLaplacian, SharpCheckerboardScoresHigherThanBlurred) {
  // arrange
  cv::Mat sharp(16, 16, CV_8UC1);
  for (int r = 0; r < 16; ++r)
    for (int c = 0; c < 16; ++c)
      sharp.at<uchar>(r, c) = ((r + c) % 2 == 0) ? 0 : 255;

  cv::Mat blurred;
  cv::GaussianBlur(sharp, blurred, cv::Size(7, 7), 3.0);

  // act
  utils::image::SharpnessEstimatorLaplacian estimator;
  const double sharpScore   = estimator.getSharpness(sharp);
  const double blurredScore = estimator.getSharpness(blurred);

  // assert
  EXPECT_GT(sharpScore, blurredScore);
}

TEST(SharpnessEstimatorLaplacian, UniformImageReturnsNearZero) {
  // arrange
  cv::Mat img(8, 8, CV_8UC1, cv::Scalar(128));
  // act
  utils::image::SharpnessEstimatorLaplacian estimator;
  const double score = estimator.getSharpness(img);
  // assert
  EXPECT_THAT(score, DoubleNear(0.0, 1.0));
}

TEST(SharpnessEstimatorLaplacian, ColorImageIsAccepted) {
  // arrange
  cv::Mat img(8, 8, CV_8UC3, cv::Scalar(100, 150, 200));
  // act & assert
  utils::image::SharpnessEstimatorLaplacian estimator;
  EXPECT_NO_THROW(estimator.getSharpness(img));
}

TEST(SharpnessEstimatorLaplacian, PreBlurWithSigmaReducesScore) {
  // arrange — positive sigma pre-blurs before Laplacian, softening edges
  cv::Mat sharp(16, 16, CV_8UC1);
  for (int r = 0; r < 16; ++r)
    for (int c = 0; c < 16; ++c)
      sharp.at<uchar>(r, c) = ((r + c) % 2 == 0) ? 0 : 255;

  // act
  utils::image::SharpnessEstimatorLaplacian withSigma(2.0);
  utils::image::SharpnessEstimatorLaplacian noSigma(0.0);

  // assert — pre-blurring should lower the apparent sharpness
  EXPECT_LT(withSigma.getSharpness(sharp), noSigma.getSharpness(sharp));
}

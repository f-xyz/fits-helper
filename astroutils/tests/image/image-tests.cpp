#include "../../include/image/image.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

using ::testing::Eq;
using ::testing::HasSubstr;

TEST(Image, RangeReturnsMinAndMax) {
  // arrange
  cv::Mat image = (cv::Mat_<uchar>(2, 2) << 10, 20, 30, 40);
  // act
  const auto result = utils::image::range(image);
  // assert
  EXPECT_THAT(result, Eq(std::make_pair(10.0, 40.0)));
}

TEST(Image, NormalizeScalesTo8BitRange) {
  // arrange
  cv::Mat image = (cv::Mat_<float>(1, 3) << 10.0f, 20.0f, 30.0f);
  // act
  const auto result = utils::image::normalize(image);
  // assert
  EXPECT_EQ(result.type(), CV_8UC1);
  ASSERT_EQ(result.total(), 3u);
  EXPECT_EQ(result.at<uchar>(0, 0), 0);
  EXPECT_EQ(result.at<uchar>(0, 1), 128);
  EXPECT_EQ(result.at<uchar>(0, 2), 255);
}

TEST(Image, SplitAndMergeRoundTrip) {
  // arrange
  std::vector<cv::Mat> channels = {
      (cv::Mat_<uchar>(1, 2) << 1, 2),
      (cv::Mat_<uchar>(1, 2) << 3, 4),
      (cv::Mat_<uchar>(1, 2) << 5, 6),
  };
  // act
  const auto merged = utils::image::merge(channels);
  const auto split = utils::image::split(merged);
  // assert
  EXPECT_EQ(split.size(), 3u);
  for (std::size_t i = 0; i < channels.size(); ++i) {
    EXPECT_EQ(split[i].rows, channels[i].rows);
    EXPECT_EQ(split[i].cols, channels[i].cols);
    EXPECT_EQ(split[i].type(), channels[i].type());
    EXPECT_EQ(cv::countNonZero(split[i] != channels[i]), 0);
  }
}

TEST(Image, LightnessReturnsLChannelForColorImage) {
  // arrange
  cv::Mat image(1, 1, CV_8UC3, cv::Scalar(0, 0, 255));
  // act
  const auto result = utils::image::lightness(image);
  // assert
  EXPECT_EQ(result.type(), CV_8UC1);
  EXPECT_EQ(result.rows, 1);
  EXPECT_EQ(result.cols, 1);
}

TEST(Image, RoiReturnsCenteredCrop) {
  // arrange
  cv::Mat image = (cv::Mat_<uchar>(4, 4) << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                   12, 13, 14, 15, 16);
  // act
  const auto result = utils::image::roi(image, 2);
  // assert
  EXPECT_EQ(result.rows, 2);
  EXPECT_EQ(result.cols, 2);
  EXPECT_EQ(result.at<uchar>(0, 0), 6);
  EXPECT_EQ(result.at<uchar>(0, 1), 7);
  EXPECT_EQ(result.at<uchar>(1, 0), 10);
  EXPECT_EQ(result.at<uchar>(1, 1), 11);
}

TEST(Image, InfoIncludesTypeDimensionsAndRange) {
  // arrange
  cv::Mat image = (cv::Mat_<uchar>(2, 2) << 1, 2, 3, 4);
  // act
  const auto result = utils::image::info(image);
  // assert
  EXPECT_THAT(result, HasSubstr("CV_8UC1 2x2 [1-4]"));
}

TEST(Image, HistogramCountsPixelsInBins) {
  // arrange
  cv::Mat image = (cv::Mat_<uchar>(2, 2) << 0, 0, 255, 255);
  // act
  const auto hist = utils::image::histogram(image, 256);
  // assert
  EXPECT_EQ(hist.size(), 256u);
  EXPECT_EQ(hist[0], 2);
  EXPECT_EQ(hist[255], 2);
}

TEST(Image, ClampRestrictsRangeAndNormalizes) {
  // arrange
  cv::Mat image = (cv::Mat_<uchar>(1, 5) << 0, 5, 10, 15, 20);
  // act
  const auto result = utils::image::clamp(image, 5, 15);
  // assert
  EXPECT_EQ(result.type(), CV_8UC1);
  ASSERT_EQ(result.total(), 5u);
  EXPECT_EQ(result.at<uchar>(0, 0), 0);
  EXPECT_EQ(result.at<uchar>(0, 1), 0);
  EXPECT_EQ(result.at<uchar>(0, 2), 128);
  EXPECT_EQ(result.at<uchar>(0, 3), 255);
  EXPECT_EQ(result.at<uchar>(0, 4), 255);
}

TEST(Image, ClahePreservesShapeAndType) {
  // arrange
  cv::Mat image = (cv::Mat_<uchar>(4, 4) << 0, 0, 0, 0, 0, 64, 64, 0, 0, 64, 64,
                   0, 0, 0, 0, 0);
  // act
  const auto result = utils::image::clahe(image, 2.0, 2);
  // assert
  EXPECT_EQ(result.type(), image.type());
  EXPECT_EQ(result.rows, image.rows);
  EXPECT_EQ(result.cols, image.cols);
}

TEST(Image, SoftRangeFindsMostPopulatedValues) {
  // arrange
  cv::Mat image =
      (cv::Mat_<uchar>(2, 5) << 10, 10, 10, 20, 20, 30, 30, 40, 40, 40);
  // act
  const auto [min, max] = utils::image::soft_range(image, 2);
  // assert
  EXPECT_EQ(min, 10);
  EXPECT_EQ(max, 40);
}

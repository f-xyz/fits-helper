#include <astroutils/image/ImageStretcher.hpp>
#include <gtest/gtest.h>

namespace {

cv::Mat makeTestImage() {
  cv::Mat image(32, 32, CV_8UC3);
  for (int row = 0; row < image.rows; ++row) {
    for (int col = 0; col < image.cols; ++col) {
      image.at<cv::Vec3b>(row, col) = {
          static_cast<unsigned char>(col * 8),
          static_cast<unsigned char>(row * 8),
          static_cast<unsigned char>((row + col) * 4)};
    }
  }
  return image;
}

} // namespace

TEST(ImageStretcher, AsinhWithoutChromaScalingReturnsColorImage) {
  astroutils::image::ImageStretcherOptions options;
  options.types = {astroutils::image::ImageStretcherOptions::Type::Asinh};
  options.scaleChroma = false;

  const auto result = astroutils::image::ImageStretcher(options).stretch(makeTestImage());

  EXPECT_EQ(result.type(), CV_8UC3);
  EXPECT_EQ(result.size(), cv::Size(32, 32));
}

TEST(ImageStretcher, ClaheCanFollowAsinh) {
  astroutils::image::ImageStretcherOptions options;
  options.types = {astroutils::image::ImageStretcherOptions::Type::Asinh,
                  astroutils::image::ImageStretcherOptions::Type::CLAHE};
  options.scaleChroma = false;

  const auto result = astroutils::image::ImageStretcher(options).stretch(makeTestImage());

  EXPECT_EQ(result.type(), CV_8UC3);
  EXPECT_EQ(result.size(), cv::Size(32, 32));
}

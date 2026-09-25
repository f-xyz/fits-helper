#include <astroutils/fits/FitsReader.h>
#include <astroutils/fits/FitsWriter.h>
#include <astroutils/fs.hpp>
#include <gtest/gtest.h>

TEST(FitsWriterTest, WriteAndReadBackFile) {
  // arrange
  cv::Mat original = cv::Mat::zeros(100, 100, CV_16UC1);
  original.at<uint16_t>(50, 50) = 12345;
  original.at<uint16_t>(10, 20) = 65535;

  // act
  std::string filename = "test_writer_output.fits";
  astroutils::fits::FitsWriter writer;
  EXPECT_TRUE(writer.write(filename, original, "RGGB"));

  // assert
  astroutils::fits::FitsReader reader;
  cv::Mat loaded = reader.read(filename);

  EXPECT_FALSE(loaded.empty());
  // Demosaicing will run if BAYERPAT is present, but reading with FitsReader demosaics RGGB into BGR if 1-channel.
  // Let's also test without Bayer pattern for exact bit-for-bit check:
  std::string filenameRaw = "test_writer_raw.fits";
  EXPECT_TRUE(writer.write(filenameRaw, original));

  // assert
  cv::Mat loadedRaw = reader.read(filenameRaw);
  EXPECT_EQ(loadedRaw.rows, original.rows);
  EXPECT_EQ(loadedRaw.cols, original.cols);
  EXPECT_EQ(loadedRaw.type(), original.type());
  EXPECT_EQ(loadedRaw.at<uint16_t>(50, 50), 12345);
  EXPECT_EQ(loadedRaw.at<uint16_t>(10, 20), 65535);

  std::filesystem::remove(filename);
  std::filesystem::remove(filenameRaw);
}

TEST(FitsWriterTest, WritesNonContiguousMultiChannelImageAsFitsCube) {
  cv::Mat parent(3, 4, CV_8UC3);
  cv::Mat image = parent(cv::Rect(1, 1, 2, 2));
  ASSERT_FALSE(image.isContinuous());
  for (int row = 0; row < image.rows; ++row) {
    for (int col = 0; col < image.cols; ++col) {
      auto &pixel = image.at<cv::Vec3b>(row, col);
      const auto base = static_cast<unsigned char>((row * image.cols + col) * 3 + 1);
      pixel = {base, static_cast<unsigned char>(base + 1),
               static_cast<unsigned char>(base + 2)};
    }
  }

  const auto path = astroutils::fs::tmpFile() + ".fits";
  astroutils::fits::FitsWriter writer;
  ASSERT_TRUE(writer.write(path, image));

  fitsfile *file = nullptr;
  int status = 0;
  ASSERT_EQ(fits_open_file(&file, path.c_str(), READONLY, &status), 0);
  int nDimensions = 0;
  long dimensions[3] = {};
  ASSERT_EQ(fits_get_img_dim(file, &nDimensions, &status), 0);
  ASSERT_EQ(fits_get_img_size(file, 3, dimensions, &status), 0);
  EXPECT_EQ(nDimensions, 3);
  EXPECT_EQ(dimensions[0], 2);
  EXPECT_EQ(dimensions[1], 2);
  EXPECT_EQ(dimensions[2], 3);

  std::array<unsigned char, 12> pixels{};
  long firstPixel[3] = {1, 1, 1};
  ASSERT_EQ(fits_read_pix(file, TBYTE, firstPixel, pixels.size(), nullptr,
                         pixels.data(), nullptr, &status), 0);
  EXPECT_EQ(pixels, (std::array<unsigned char, 12>{1, 4, 7, 10,
                                                   2, 5, 8, 11,
                                                   3, 6, 9, 12}));
  EXPECT_EQ(fits_close_file(file, &status), 0);
  std::filesystem::remove(path);
}

TEST(FitsWriterTest, RejectsUnsupportedDepth) {
  cv::Mat image(2, 2, CV_16FC1);
  astroutils::fits::FitsWriter writer;
  EXPECT_THROW(writer.write(astroutils::fs::tmpFile() + ".fits", image),
               std::runtime_error);
}

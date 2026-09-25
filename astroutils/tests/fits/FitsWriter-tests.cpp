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
  utils::fits::FitsWriter writer;
  EXPECT_TRUE(writer.write(filename, original, "RGGB"));

  // assert
  utils::fits::FitsReader reader;
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
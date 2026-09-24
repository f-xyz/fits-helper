#include <astroutils/fits/FitsReader.h>
#include <astroutils/fs.hpp>
#include "helpers.hpp"

TEST(FitsReader, InvalidMemoryReturnsEmptyMat) {
  // arrange
  FitsReader reader;
  std::array<unsigned char, 4> invalid = {0, 1, 2, 3};
  // act
  const auto result = reader.read(invalid.data(), invalid.size());
  // assert
  EXPECT_TRUE(result.empty());
}

TEST(FitsReader, ReadsUShortImageFromFile) {
  // arrange
  const auto path = utils::fs::tmpFile();
  createFitsFile(path);

  // act
  FitsReader reader;
  const auto image = reader.read(path);

  // assert
  EXPECT_FALSE(image.empty());
  EXPECT_EQ(image.rows, 2);
  EXPECT_EQ(image.cols, 2);
  EXPECT_EQ(image.channels(), 1);
  EXPECT_EQ(image.at<unsigned short>(0, 0), 1);
  EXPECT_EQ(image.at<unsigned short>(0, 1), 2);
  EXPECT_EQ(image.at<unsigned short>(1, 0), 3);
  EXPECT_EQ(image.at<unsigned short>(1, 1), 4);

  std::filesystem::remove(path);
}

TEST(FitsReader, ReadsUShortImageFromMemoryBuffer) {
  // arrange
  const auto path = utils::fs::tmpFile();
  createFitsFile(path);
  auto bytes = utils::fs::readFile(path);
  std::filesystem::remove(path);

  // act
  FitsReader reader;
  const auto image = reader.read(bytes.data(), bytes.size());

  // assert
  EXPECT_FALSE(image.empty());
  EXPECT_EQ(image.rows, 2);
  EXPECT_EQ(image.cols, 2);
  EXPECT_EQ(image.channels(), 1);
  EXPECT_EQ(image.at<unsigned short>(0, 0), 1);
  EXPECT_EQ(image.at<unsigned short>(0, 1), 2);
  EXPECT_EQ(image.at<unsigned short>(1, 0), 3);
  EXPECT_EQ(image.at<unsigned short>(1, 1), 4);
}

////////////////////////////////////////
// Error paths /////////////////////////
////////////////////////////////////////

TEST(FitsReader, InvalidFilePathReturnsEmptyMat) {
  FitsReader reader;
  const auto result = reader.read("/nonexistent/path/to/file.fits");
  EXPECT_TRUE(result.empty());
}

TEST(FitsReader, FileWithNoImageHduReturnsEmptyMat) {
  const auto path = utils::fs::tmpFile();
  createTableOnlyFitsFile(path);

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_TRUE(result.empty());
  std::filesystem::remove(path);
}

TEST(FitsReader, MemoryWithNoImageHduReturnsEmptyMat) {
  const auto path = utils::fs::tmpFile();
  createTableOnlyFitsFile(path);
  auto bytes = utils::fs::readFile(path);
  std::filesystem::remove(path);

  FitsReader reader;
  const auto result = reader.read(bytes.data(), bytes.size());

  EXPECT_TRUE(result.empty());
}
#include <astroutils/fits/FitsReader.h>
#include <astroutils/fs.hpp>
#include "helpers.hpp"

TEST(FitsReader, InvalidMemoryThrows) {
  // arrange
  astroutils::fits::FitsReader reader;
  std::array<unsigned char, 4> invalid = {0, 1, 2, 3};
  // act
  // assert
  EXPECT_THROW(reader.read(invalid.data(), invalid.size()), std::runtime_error);
}

TEST(FitsReader, ReadsUShortImageFromFile) {
  // arrange
  const auto path = astroutils::fs::tmpFile();
  createFitsFile(path);

  // act
  astroutils::fits::FitsReader reader;
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
  const auto path = astroutils::fs::tmpFile();
  createFitsFile(path);
  auto bytes = astroutils::fs::readFile(path);
  std::filesystem::remove(path);

  // act
  astroutils::fits::FitsReader reader;
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

TEST(FitsReader, InvalidFilePathThrows) {
  astroutils::fits::FitsReader reader;
  EXPECT_THROW(reader.read("/nonexistent/path/to/file.fits"), std::runtime_error);
}

TEST(FitsReader, FileWithNoImageHduThrows) {
  const auto path = astroutils::fs::tmpFile();
  createTableOnlyFitsFile(path);

  astroutils::fits::FitsReader reader;
  EXPECT_THROW(reader.read(path), std::runtime_error);
  std::filesystem::remove(path);
}

TEST(FitsReader, MemoryWithNoImageHduThrows) {
  const auto path = astroutils::fs::tmpFile();
  createTableOnlyFitsFile(path);
  auto bytes = astroutils::fs::readFile(path);
  std::filesystem::remove(path);

  astroutils::fits::FitsReader reader;
  EXPECT_THROW(reader.read(bytes.data(), bytes.size()), std::runtime_error);
}
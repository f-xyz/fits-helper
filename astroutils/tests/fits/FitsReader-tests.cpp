#include "fits/FitsReader.h"
#include "fs.hpp"
#include <array>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

void createFitsFile(const std::string &path) {
  int status = 0;
  fitsfile *fptr = nullptr;
  fits_create_file(&fptr, path.c_str(), &status);

  long naxes[] = {2, 2};
  fits_create_img(fptr, USHORT_IMG, 2, naxes, &status);

  long firstpix[] = {1, 1};
  std::vector<unsigned short> pixels = {1, 2, 3, 4};
  const auto size = pixels.size();
  const auto data = pixels.data();
  fits_write_pix(fptr, TUSHORT, firstpix, size, data, &status);
  fits_close_file(fptr, &status);
}

std::vector<std::byte> createMemoryFitsFile() {
  const auto path = utils::fs::tmpFile();
  createFitsFile(path);

  auto bytes = utils::fs::readFile(path);
  std::filesystem::remove(path);

  return bytes;
}

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
  // auto bytes = readFileBytes(path);
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

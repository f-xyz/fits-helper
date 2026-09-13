#include "fits/FitsReader.h"
#include "fs.hpp"

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

// Helper: create a FITS file with a specific pixel type
template <typename T>
void createFitsFileTyped(const std::string &path, int fitsImgType, int fitsIoType,
                         long width, long height, std::vector<T> pixels) {
  int status = 0;
  fitsfile *fptr = nullptr;
  fits_create_file(&fptr, path.c_str(), &status);
  long naxes[] = {width, height};
  fits_create_img(fptr, fitsImgType, 2, naxes, &status);
  long firstpix[] = {1, 1};
  fits_write_pix(fptr, fitsIoType, firstpix, (long)pixels.size(), pixels.data(), &status);
  fits_close_file(fptr, &status);
}

// Helper: create a FITS file with BAYERPAT keyword
void createBayerFitsFile(const std::string &path, const std::string &bayerPat) {
  int status = 0;
  fitsfile *fptr = nullptr;
  fits_create_file(&fptr, path.c_str(), &status);
  long naxes[] = {4, 4};
  fits_create_img(fptr, USHORT_IMG, 2, naxes, &status);
  fits_write_key(fptr, TSTRING, "BAYERPAT",
                 const_cast<char *>(bayerPat.c_str()), nullptr, &status);
  long firstpix[] = {1, 1};
  std::vector<unsigned short> pixels(16, 128);
  fits_write_pix(fptr, TUSHORT, firstpix, 16, pixels.data(), &status);
  fits_close_file(fptr, &status);
}

// Helper: create a FITS file with a 0-axis primary HDU (no 2D image)
void createTableOnlyFitsFile(const std::string &path) {
  int status = 0;
  fitsfile *fptr = nullptr;
  fits_create_file(&fptr, path.c_str(), &status);
  fits_create_img(fptr, BYTE_IMG, 0, nullptr, &status);
  fits_close_file(fptr, &status);
}

// -----------------------------------------------------------------------
// Original tests
// -----------------------------------------------------------------------

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

// -----------------------------------------------------------------------
// Error paths
// -----------------------------------------------------------------------

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

// -----------------------------------------------------------------------
// Bayer demosaic paths (getBayerCode + demosaic branches)
// -----------------------------------------------------------------------

TEST(FitsReader, DemosaicsRGGBPattern) {
  const auto path = utils::fs::tmpFile();
  createBayerFitsFile(path, "RGGB");

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.channels(), 3); // demosaiced → BGR
  std::filesystem::remove(path);
}

TEST(FitsReader, DemosaicsBGGRPattern) {
  const auto path = utils::fs::tmpFile();
  createBayerFitsFile(path, "BGGR");

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.channels(), 3);
  std::filesystem::remove(path);
}

TEST(FitsReader, DemosaicsGRBGPattern) {
  const auto path = utils::fs::tmpFile();
  createBayerFitsFile(path, "GRBG");

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.channels(), 3);
  std::filesystem::remove(path);
}

TEST(FitsReader, DemosaicsGBRGPattern) {
  const auto path = utils::fs::tmpFile();
  createBayerFitsFile(path, "GBRG");

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.channels(), 3);
  std::filesystem::remove(path);
}

TEST(FitsReader, UnknownBayerPatternReturnsRawImage) {
  const auto path = utils::fs::tmpFile();
  createBayerFitsFile(path, "XYZW"); // unsupported pattern → fallback

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.channels(), 1); // raw, not demosaiced
  std::filesystem::remove(path);
}

// -----------------------------------------------------------------------
// Pixel format paths (getDataTypes switch branches)
// -----------------------------------------------------------------------

TEST(FitsReader, ReadsByteImage) {
  const auto path = utils::fs::tmpFile();
  std::vector<unsigned char> pixels = {10, 20, 30, 40};
  createFitsFileTyped(path, BYTE_IMG, TBYTE, 2, 2, pixels);

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.type(), CV_8UC1);
  EXPECT_EQ(result.at<uchar>(0, 0), 10);
  std::filesystem::remove(path);
}

TEST(FitsReader, ReadsShortImage) {
  const auto path = utils::fs::tmpFile();
  std::vector<short> pixels = {-100, 200, 300, 400};
  createFitsFileTyped(path, SHORT_IMG, TSHORT, 2, 2, pixels);

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.type(), CV_16SC1);
  EXPECT_EQ(result.at<short>(0, 0), -100);
  std::filesystem::remove(path);
}

TEST(FitsReader, ReadsLongImage) {
  const auto path = utils::fs::tmpFile();
  std::vector<int> pixels = {1000, 2000, 3000, 4000};
  createFitsFileTyped(path, LONG_IMG, TLONG, 2, 2, pixels);

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.type(), CV_32SC1);
  std::filesystem::remove(path);
}

TEST(FitsReader, ReadsFloatImage) {
  const auto path = utils::fs::tmpFile();
  std::vector<float> pixels = {1.5f, 2.5f, 3.5f, 4.5f};
  createFitsFileTyped(path, FLOAT_IMG, TFLOAT, 2, 2, pixels);

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.type(), CV_32FC1);
  EXPECT_FLOAT_EQ(result.at<float>(0, 0), 1.5f);
  std::filesystem::remove(path);
}

TEST(FitsReader, ReadsDoubleImage) {
  const auto path = utils::fs::tmpFile();
  std::vector<double> pixels = {1.1, 2.2, 3.3, 4.4};
  createFitsFileTyped(path, DOUBLE_IMG, TDOUBLE, 2, 2, pixels);

  FitsReader reader;
  const auto result = reader.read(path);

  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.type(), CV_64FC1);
  EXPECT_DOUBLE_EQ(result.at<double>(0, 0), 1.1);
  std::filesystem::remove(path);
}

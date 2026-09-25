#include <astroutils/exception/fail.hpp>
#include <astroutils/fits/FitsWriter.h>
#include <astroutils/fits/common.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <limits>
#include <vector>

namespace astroutils::fits {

using astroutils::exception::fail;

bool FitsWriter::write(const std::filesystem::path &path,
                       const cv::Mat &image) {
  return write(path, image, "");
}

bool FitsWriter::write(const std::filesystem::path &path, const cv::Mat &image,
                       std::string_view bayerPattern) {
  if (image.empty()) {
    fail("cv::Mat is empty.");
  }

  const int bitsPerPixel = getBitsPerPixel(image.depth());
  const int fitsDataType = getDataType(image.depth());

  FitsFilePtr file;
  int status = 0;

  // Prefix ! to force overwrite if file already exists
  const std::string filename = "!" + path.string();
  if (fits_create_file(&file.ptr, filename.c_str(), &status)) {
    fail("Failed to create file {}: {}.", path.string(), status);
  }

  createFitsImage(file.ptr, image, bitsPerPixel, status);
  writePixels(file.ptr, image, fitsDataType, status);
  writeBayerPattern(file.ptr, bayerPattern, status);

  if (status != 0) {
    fail("Failed to write FITS metadata: {}.", status);
  }

  return true;
}

void FitsWriter::createFitsImage(fitsfile *file, const cv::Mat &image,
                                 int bitsPerPixel, int &status) const {
  const int channels = image.channels();
  const int nDimensions = channels == 1 ? 2 : 3;
  long axes[3] = {image.cols, image.rows, channels};

  if (fits_create_img(file, bitsPerPixel, nDimensions, axes, &status)) {
    fail("Failed to create image HDU: {}.", status);
  }
}

void FitsWriter::writePixels(fitsfile *file, const cv::Mat &image,
                             int fitsDataType, int &status) const {
  const size_t channels = static_cast<size_t>(image.channels());
  const size_t maxElements = static_cast<size_t>(std::numeric_limits<long>::max());

  if (image.total() > maxElements / channels) {
    fail("Image is too large to write to a FITS file.");
  }

  std::vector<std::byte> packedPixels;
  const void *pixels = image.data;
  const size_t nElements = image.total() * channels;

  if (image.channels() != 1 || !image.isContinuous()) {
    packedPixels = packPixels(image);
    pixels = packedPixels.data();
  }

  long firstPixel[3] = {1, 1, 1};
  if (fits_write_pix(file, fitsDataType, firstPixel,
                     static_cast<long>(nElements),
                     const_cast<void *>(pixels), &status)) {
    fail("Failed to write pixels: {}.", status);
  }
}

std::vector<std::byte> FitsWriter::packPixels(const cv::Mat &image) const {
  const size_t scalarSize = image.elemSize1();
  const size_t elementCount = image.total() * static_cast<size_t>(image.channels());
  std::vector<std::byte> packed(elementCount * scalarSize);

  if (image.channels() == 1) {
    for (int y = 0; y < image.rows; ++y) {
      std::memcpy(packed.data() +
                      static_cast<size_t>(y) * image.cols * scalarSize,
                  image.ptr(y), static_cast<size_t>(image.cols) * scalarSize);
    }
    return packed;
  }

  const size_t planeSize = image.total();
  for (int y = 0; y < image.rows; ++y) {
    const auto *source = image.ptr<std::byte>(y);
    for (int x = 0; x < image.cols; ++x) {
      for (int channel = 0; channel < image.channels(); ++channel) {
        const size_t sourceOffset = static_cast<size_t>(x) * image.elemSize() +
                                    static_cast<size_t>(channel) * scalarSize;
        const size_t destinationIndex =
            static_cast<size_t>(channel) * planeSize +
            static_cast<size_t>(y) * image.cols + x;
        std::memcpy(packed.data() + destinationIndex * scalarSize,
                    source + sourceOffset, scalarSize);
      }
    }
  }

  return packed;
}

void FitsWriter::writeBayerPattern(fitsfile *file,
                                   std::string_view bayerPattern,
                                   int &status) const {
  if (bayerPattern.empty()) {
    return;
  }

  const std::string pattern(bayerPattern);
  fits_write_key(file, TSTRING, "BAYERPAT", const_cast<char *>(pattern.c_str()),
                 "Bayer color pattern", &status);
}

int FitsWriter::getBitsPerPixel(int cvType) const {
  switch (cvType) {
    case CV_8U:   return BYTE_IMG;
    case CV_8S:   return SBYTE_IMG;
    case CV_16U:  return USHORT_IMG;
    case CV_16S:  return SHORT_IMG;
    case CV_32S:  return LONG_IMG;
    case CV_32F:  return FLOAT_IMG;
    case CV_64F:  return DOUBLE_IMG;
    default:      fail("Unsupported OpenCV image depth: {}.", cvType);
  }
}

int FitsWriter::getDataType(int cvType) const {
  switch (cvType) {
    case CV_8U:   return TBYTE;
    case CV_8S:   return TSBYTE;
    case CV_16U:  return TUSHORT;
    case CV_16S:  return TSHORT;
    case CV_32S:  return TLONG;
    case CV_32F:  return TFLOAT;
    case CV_64F:  return TDOUBLE;
    default:      fail("Unsupported OpenCV image depth: {}.", cvType);
  }
}

} // namespace astroutils::fits

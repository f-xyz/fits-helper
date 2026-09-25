#include <astroutils/fits/common.h>
#include <astroutils/fits/FitsWriter.h>
#include <astroutils/exception/fail.hpp>
#include <cstdlib>
#include <filesystem>

namespace utils::fits {

using utils::exception::fail;

bool FitsWriter::write(const std::filesystem::path &path,
                       const cv::Mat &image) {
  return write(path, image, "");
}

bool FitsWriter::write(const std::filesystem::path &path, const cv::Mat &image,
                       std::string_view bayerPattern) {
  if (image.empty()) {
    fail("cv::Mat is empty.");
  }

  FitsFilePtr file;
  int status = 0;

  // Prefix ! to force overwrite if file already exists
  const std::string filename = "!" + path.string();
  if (fits_create_file(&file.ptr, filename.c_str(), &status)) {
    fail("Failed to create file {}: {}.", path.string(), status);
  }

  const int depth = image.depth();
  const int bitsPerPIxel = getBitsPerPixel(depth);
  const int fitsDataType = getDataType(depth);

  long nAxes[2] = {image.cols, image.rows};
  if (fits_create_img(file.ptr, bitsPerPIxel, 2, nAxes, &status)) {
    fail("Failed to create image HDU: {}.", status);
  }

  long pixel[2] = {1, 1};
  long nElements = static_cast<long>(image.cols) * image.rows;
  if (fits_write_pix(file.ptr, fitsDataType, pixel, nElements, image.data, &status)) {
    fail("Failed to write pixels: {}.", status);
  }

  if (!bayerPattern.empty()) {
    fits_write_key(file.ptr, TSTRING, "BAYERPAT",
                   const_cast<char *>(bayerPattern.data()),
                   "Bayer color pattern", &status);
  }

  return status == 0;
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
    default:      return BYTE_IMG;
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
    default:      return TBYTE;
  }
}

} // namespace utils::fits
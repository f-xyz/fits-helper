#include <astroutils/fits/FitsReader.h>
#include <astroutils/exception/fail.hpp>
#include <opencv2/imgproc.hpp>
#include <string_view>
#include <vector>

namespace astroutils::fits {

using astroutils::exception::fail;

cv::Mat FitsReader::read(const std::filesystem::path &path) {
  FitsFilePtr file;

  int status = 0;
  if (fits_open_file(&file.ptr, path.c_str(), READONLY, &status)) {
    fail("Failed to open file.");
  }

  if (!findFirstImageHdu(file.ptr)) {
    fail("No 2D image HDU was found.");
  }

  const FitsImageParams params = getImageParams(file.ptr);
  const cv::Mat image = readImage(file.ptr, params);

  return demosaic(image, params.bayerPattern);
}

cv::Mat FitsReader::read(void *data, size_t size) {
  FitsFilePtr file;

  int status = 0;
  if (fits_open_memfile(&file.ptr, "", READONLY, &data,
    &size, 0, nullptr, &status)) {
    fail("Failed to open file.");
  }

  if (!findFirstImageHdu(file.ptr)) {
    fail("No 2D image HDU was found.");
  }

  const FitsImageParams params = getImageParams(file.ptr);
  const cv::Mat image = readImage(file.ptr, params);

  return demosaic(image, params.bayerPattern);
}

bool FitsReader::findFirstImageHdu(fitsfile *fptr) {
  int nHdu = 1;
  int hduType = 0;

  while (true) {
    int moveStatus = 0;
    if (fits_movabs_hdu(fptr, nHdu, &hduType, &moveStatus) != 0) {
      return false;
    }

    if (hduType == IMAGE_HDU) {
      int nDimensions = 0;
      int getImgDimStatus = 0;
      fits_get_img_dim(fptr, &nDimensions, &getImgDimStatus);

      if (getImgDimStatus == 0 && nDimensions >= 2) {
        return true; // Found the image!
      }
    }

    ++nHdu;
  }

  return false;
}

FitsImageParams FitsReader::getImageParams(fitsfile *fptr) {
  FitsImageParams result;
  int status = 0;

  // Dimensions
  fits_get_img_param(fptr, 2, nullptr, &result.nDimensions, result.dimensions, &status);
  if (status != 0) {
    fail("Failed to get image parameters: {}.", status);
  }

  // Bits per pixel
  fits_get_img_equivtype(fptr, &result.bitsPerPixel, &status);
  if (status != 0) {
    fail("Failed to determine the image data type: {}.", status);
  }

  // Bayer array pattern
  fits_read_key(fptr, TSTRING, "BAYERPAT", &result.bayerPattern, nullptr, &status);
  if (status != 0) {
    status = 0;
    result.bayerPattern[0] = '\0';
  }

  return result;
}

cv::Mat FitsReader::readImage(fitsfile *fptr, const FitsImageParams &params) {
  const auto [width, height] = params.dimensions;
  const auto [cvType, fitsType] = getDataType(params.bitsPerPixel);
  cv::Mat image(static_cast<int>(height), static_cast<int>(width), cvType);

  int status = 0;
  std::vector<long> pixel(params.nDimensions, 1); // FITS starts counting from 1
  fits_read_pix(fptr, fitsType, pixel.data(), width * height, nullptr,
                image.data, nullptr, &status);

  if (status != 0) {
    fail("Failed to read image pixels: {}.", status);
  }

  return image;
}

FitsDataType FitsReader::getDataType(int bitsPerPixel) const {
  switch (bitsPerPixel) {
    case BYTE_IMG:   return {CV_8UC1, TBYTE};
    case SBYTE_IMG:  return {CV_8SC1, TSBYTE};
    case SHORT_IMG:  return {CV_16SC1, TSHORT};
    case USHORT_IMG: return {CV_16UC1, TUSHORT};
    case LONG_IMG:   return {CV_32SC1, TLONG};
    case FLOAT_IMG:  return {CV_32FC1, TFLOAT};
    case DOUBLE_IMG: return {CV_64FC1, TDOUBLE};
    default: fail("Unsupported image bit depth: {}.", bitsPerPixel);
  };
}

cv::Mat FitsReader::demosaic(const cv::Mat &image, const char *bayer) {
  if (bayer[0] != 0) {
    cv::Mat result;
    const int bayerCode = getBayerCode(bayer);
    cv::cvtColor(image, result, bayerCode);
    return result;
  }

  return image;
}

int FitsReader::getBayerCode(std::string_view pattern) const {
  if (pattern == "RGGB") return cv::COLOR_BayerRG2BGR_EA;
  if (pattern == "BGGR") return cv::COLOR_BayerBG2BGR_EA;
  if (pattern == "GRBG") return cv::COLOR_BayerGR2BGR_EA;
  if (pattern == "GBRG") return cv::COLOR_BayerGB2BGR_EA;

  fail("Unsupported Bayer pattern: {}.", pattern);
}

} // namespace astroutils::fits
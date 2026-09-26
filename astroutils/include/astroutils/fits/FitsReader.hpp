#pragma once

#include <astroutils/fits/fits-common.hpp>
#include <cstddef>
#include <filesystem>
#include <fitsio.h>
#include <opencv2/core.hpp>
#include <string_view>

namespace astroutils::fits {

class FitsReader {
public:
  cv::Mat read(const std::filesystem::path &path);
  cv::Mat read(void *data, size_t size);

private:
  bool findFirstImageHdu(fitsfile *fptr);
  FitsImageParams getImageParams(fitsfile *fptr);
  cv::Mat readImage(fitsfile *fptr, const FitsImageParams &params);
  FitsDataType getDataType(int bitsPerPixel) const;
  cv::Mat demosaic(const cv::Mat &image, const char *bayer);
  int getBayerCode(std::string_view pattern) const;
};

} // namespace astroutils::fits
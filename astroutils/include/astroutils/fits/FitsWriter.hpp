#pragma once

#include <cstddef>
#include <filesystem>
#include <fitsio.h>
#include <opencv2/core.hpp>
#include <string_view>
#include <vector>

namespace astroutils::fits {

class FitsWriter {
public:
  bool write(const std::filesystem::path &path, const cv::Mat &image);
  bool write(const std::filesystem::path &path, const cv::Mat &image,
             std::string_view bayerPattern);

private:
  int getBitsPerPixel(int cvType) const;
  int getDataType(int cvType) const;
  void createFitsImage(fitsfile *file, const cv::Mat &image, int bitsPerPixel, int &status) const;
  void writePixels(fitsfile *file, const cv::Mat &image, int fitsDataType, int &status) const;
  void writeBayerPattern(fitsfile *file, std::string_view bayerPattern, int &status) const;
  std::vector<std::byte> packPixels(const cv::Mat &image) const;
};

} // namespace astroutils::fits

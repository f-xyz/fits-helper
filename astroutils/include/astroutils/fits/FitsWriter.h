#pragma once

#include <fitsio.h>
#include <opencv2/core.hpp>

namespace astroutils::fits {

class FitsWriter {
public:
  bool write(const std::filesystem::path &path, const cv::Mat &image);
  bool write(const std::filesystem::path &path, const cv::Mat &image,
             std::string_view bayerPattern);

private:
  int getBitsPerPixel(int cvType) const;
  int getDataType(int cvType) const;
};

} // namespace astroutils::fits
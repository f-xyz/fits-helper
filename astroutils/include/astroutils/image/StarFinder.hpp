#pragma once

#include <vector>
#include <optional>
#include <opencv2/core.hpp>

namespace astroutils::stars {

struct Star {
  cv::Point2f center;
  double fwhm = 0.0;
  double eccentricity = 0.0;
  double fwhmX = 0.0;
  double fwhmY = 0.0;
  double flux = 0.0;
  double peak = 0.0;
  double semiMajor = 0.0;
  double semiMinor = 0.0;
  double orientation = 0.0; // Angle in radians
  cv::Rect bbox;
};

struct StarFinderOptions {
  double thresholdSigma = 3.0; // Detection threshold in stddev above background
  int minArea = 3;            // Minimum connected component size in pixels
  int maxArea = 1000;         // Maximum connected component size in pixels
  int maxStars = 0;           // Maximum number of stars to return (0 = unlimited)
  int starMargin = 5;         // Margin from image border
};

class StarFinder {
public:
  explicit StarFinder(StarFinderOptions options = {}) : options_(options) {}

  void setOptions(const StarFinderOptions &options) { options_ = options; }
  [[nodiscard]] const StarFinderOptions &getOptions() const { return options_; }

  [[nodiscard]] std::vector<Star> findStars(const cv::Mat &image) const;

private:
  [[nodiscard]] cv::Mat preprocessImage(const cv::Mat &image) const;
  [[nodiscard]] cv::Mat createDetectionMask(const cv::Mat &floatImg, double &outBackground) const;
  [[nodiscard]] std::optional<Star> processComponent(const cv::Mat &floatImg, double background,
                                                     const cv::Mat &stats, int componentId) const;
  void limitStars(std::vector<Star> &stars) const;

  StarFinderOptions options_;
};

} // namespace astroutils::stars

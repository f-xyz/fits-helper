////////////////////////////////////////
// GENERATED ///////////////////////////
////////////////////////////////////////

#pragma once

#include <cstdint>
#include <opencv2/core.hpp>

namespace astroutils::stars {

enum class BackgroundMethod : std::uint8_t {
  Constant,    // Constant background estimated using robust sigma-clipping / median
  GridInterpolation // Grid-based local background estimation with bilinear interpolation
};

struct BackgroundOptions {
  BackgroundMethod method = BackgroundMethod::Constant;
  int gridSize = 64;           // Grid box size in pixels (for GridInterpolation)
  int maxIterations = 5;       // Max iterations for sigma clipping
  double sigmaThreshold = 3.0; // Sigma threshold for outlier rejection in background estimation
};

struct BackgroundResult {
  cv::Mat subtracted; // Image with background subtracted
  cv::Mat background; // Estimated 2D background map (same size and depth float as image)
  double meanBackground = 0.0;
  double stdDevBackground = 0.0;
};

class SkyBackgroundEstimator {
public:
  explicit SkyBackgroundEstimator(BackgroundOptions options = {}) : options_(options) {}

  void setOptions(const BackgroundOptions &options) { options_ = options; }
  [[nodiscard]] const BackgroundOptions &getOptions() const { return options_; }

  [[nodiscard]] BackgroundResult removeBackground(const cv::Mat &image) const;

  // Helper method to perform iterative sigma clipping on pixel data
  static void computeSigmaClippedStats(const cv::Mat &singleChannelFloatImg,
                                       double &outMean,
                                       double &outStdDev,
                                       double sigmaThreshold = 3.0,
                                       int maxIterations = 5);

private:
  BackgroundOptions options_;
};

} // namespace astroutils::stars

////////////////////////////////////////
// GENERATED ///////////////////////////
////////////////////////////////////////

#include <astroutils/image/SkyBackgroundEstimator.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

namespace astroutils::stars {

void SkyBackgroundEstimator::computeSigmaClippedStats(const cv::Mat &singleChannelFloatImg,
                                                      double &outMean,
                                                      double &outStdDev,
                                                      double sigmaThreshold,
                                                      int maxIterations) {
  // Empty tiles have no estimate; define both outputs to keep callers simple.
  if (singleChannelFloatImg.empty()) {
    outMean = 0.0;
    outStdDev = 0.0;
    return;
  }

  // Work on a compact copy because clipping removes samples each iteration.
  // Continuous OpenCV matrices need only one copy; for an ROI with row gaps,
  // append each row separately so padding bytes are never treated as pixels.
  std::vector<float> pixels;
  pixels.reserve(singleChannelFloatImg.total());

  if (singleChannelFloatImg.isContinuous()) {
    const float *ptr = singleChannelFloatImg.ptr<float>();
    pixels.assign(ptr, ptr + singleChannelFloatImg.total());
  } else {
    for (int r = 0; r < singleChannelFloatImg.rows; ++r) {
      const float *ptr = singleChannelFloatImg.ptr<float>(r);
      pixels.insert(pixels.end(), ptr, ptr + singleChannelFloatImg.cols);
    }
  }

  double mean = 0.0;
  double stdDev = 0.0;

  // Recompute population mean and standard deviation after each clipping pass.
  // Bright stars and other outliers can bias an ordinary mean upward; removing
  // samples far from the current estimate makes this a more robust sky measure.
  for (int iter = 0; iter < maxIterations; ++iter) {
    if (pixels.empty()) {
      break;
    }

    double sum = 0.0;
    for (float val : pixels) {
      sum += val;
    }
    mean = sum / static_cast<double>(pixels.size());

    double sqSum = 0.0;
    for (float val : pixels) {
      double diff = val - mean;
      sqSum += diff * diff;
    }
    stdDev = std::sqrt(sqSum / static_cast<double>(pixels.size()));

    // With effectively constant data, another clipping pass cannot improve
    // the estimate and may be numerically unstable.
    if (stdDev < 1e-6) {
      break;
    }

    // Retain values inside the symmetric sigma interval. Bounds are inclusive,
    // so samples exactly on the clipping threshold remain in the estimate.
    std::vector<float> nextPixels;
    nextPixels.reserve(pixels.size());
    double lowerBound = mean - sigmaThreshold * stdDev;
    double upperBound = mean + sigmaThreshold * stdDev;

    for (float val : pixels) {
      if (val >= lowerBound && val <= upperBound) {
        nextPixels.push_back(val);
      }
    }

    if (nextPixels.size() == pixels.size()) {
      break; // Converged
    }
    pixels = std::move(nextPixels);
  }

  outMean = mean;
  outStdDev = stdDev;
}

BackgroundResult SkyBackgroundEstimator::removeBackground(const cv::Mat &image) const {
  BackgroundResult result;

  // Return an empty result for empty input rather than constructing invalid
  // OpenCV matrices or attempting to estimate statistics.
  if (image.empty()) {
    return result;
  }

  cv::Mat gray;
  // Estimate the sky on luminance for color input; a single-channel image is
  // already suitable. As in the star finder, a four-channel alpha is ignored.
  if (image.channels() == 3 || image.channels() == 4) {
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
  } else {
    gray = image;
  }

  cv::Mat floatImg;
  // Keep sky estimates, interpolation, and subtraction in floating point so
  // fractional background values are preserved for integer source images.
  gray.convertTo(floatImg, CV_32F);

  if (options_.method == BackgroundMethod::Constant) {
    // One robust statistic models the sky as a spatially uniform level.
    double meanBg = 0.0;
    double stdDevBg = 0.0;
    computeSigmaClippedStats(floatImg, meanBg, stdDevBg, options_.sigmaThreshold, options_.maxIterations);

    result.meanBackground = meanBg;
    result.stdDevBackground = stdDevBg;
    // Materialize the scalar estimate as a per-pixel map so both methods share
    // the same subtraction path below.
    result.background = cv::Mat(floatImg.size(), CV_32F, cv::Scalar(static_cast<float>(meanBg)));
  } else {
    // GridInterpolation estimates local sky levels, then interpolates between
    // them to model gradual illumination or vignetting changes across the image.
    int step = std::max(8, options_.gridSize);
    // Round grid dimensions up so the rightmost and bottommost partial tiles
    // are included. Enforce a minimum tile width to avoid tiny unstable samples.
    int gridCols = (floatImg.cols + step - 1) / step;
    int gridRows = (floatImg.rows + step - 1) / step;

    cv::Mat gridBg(gridRows, gridCols, CV_32F);
    double totalMeanSum = 0.0;
    double totalStdDevSum = 0.0;
    int count = 0;

    for (int gr = 0; gr < gridRows; ++gr) {
      for (int gc = 0; gc < gridCols; ++gc) {
        // Clip edge tiles to the image bounds; all remaining tiles use the
        // same sigma-clipped estimator as the constant-background method.
        int x = gc * step;
        int y = gr * step;
        int w = std::min(step, floatImg.cols - x);
        int h = std::min(step, floatImg.rows - y);

        cv::Mat tile = floatImg(cv::Rect(x, y, w, h));
        double tileMean = 0.0;
        double tileStdDev = 0.0;
        computeSigmaClippedStats(tile, tileMean, tileStdDev,
                                 options_.sigmaThreshold,
                                 options_.maxIterations);

        gridBg.at<float>(gr, gc) = static_cast<float>(tileMean);
        // These summary fields are averages of per-tile estimates, so each
        // tile contributes equally even when an edge tile is smaller.
        totalMeanSum += tileMean;
        totalStdDevSum += tileStdDev;
        count++;
      }
    }

    result.meanBackground = totalMeanSum / count;
    result.stdDevBackground = totalStdDevSum / count;

    // Expand sparse tile estimates into a smooth full-resolution background
    // surface. Bilinear interpolation avoids the hard boundaries that would
    // result from assigning one constant level to each tile.
    cv::resize(gridBg, result.background, floatImg.size(), 0, 0, cv::INTER_LINEAR);
  }

  // Remove the estimated sky and clamp negative residuals to zero. The clamp
  // prevents undersubtracted numerical noise from appearing as negative flux.
  cv::subtract(floatImg, result.background, result.subtracted);
  cv::threshold(result.subtracted, result.subtracted, 0, 0, cv::THRESH_TOZERO);

  return result;
}

} // namespace astroutils::stars

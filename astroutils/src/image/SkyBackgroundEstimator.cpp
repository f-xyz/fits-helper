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
  if (singleChannelFloatImg.empty()) {
    outMean = 0.0;
    outStdDev = 0.0;
    return;
  }

  // Flatten pixel data into vector for iterative clipping
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

    if (stdDev < 1e-6) {
      break;
    }

    // Filter outliers beyond mean +/- sigmaThreshold * stdDev
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

  if (image.empty()) {
    return result;
  }

  cv::Mat gray;
  if (image.channels() == 3 || image.channels() == 4) {
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
  } else {
    gray = image;
  }

  cv::Mat floatImg;
  gray.convertTo(floatImg, CV_32F);

  if (options_.method == BackgroundMethod::Constant) {
    double meanBg = 0.0;
    double stdDevBg = 0.0;
    computeSigmaClippedStats(floatImg, meanBg, stdDevBg, options_.sigmaThreshold, options_.maxIterations);

    result.meanBackground = meanBg;
    result.stdDevBackground = stdDevBg;
    result.background = cv::Mat(floatImg.size(), CV_32F, cv::Scalar(static_cast<float>(meanBg)));
  } else {
    // GridInterpolation method
    int step = std::max(8, options_.gridSize);
    int gridCols = (floatImg.cols + step - 1) / step;
    int gridRows = (floatImg.rows + step - 1) / step;

    cv::Mat gridBg(gridRows, gridCols, CV_32F);
    double totalMeanSum = 0.0;
    double totalStdDevSum = 0.0;
    int count = 0;

    for (int gr = 0; gr < gridRows; ++gr) {
      for (int gc = 0; gc < gridCols; ++gc) {
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
        totalMeanSum += tileMean;
        totalStdDevSum += tileStdDev;
        count++;
      }
    }

    result.meanBackground = totalMeanSum / count;
    result.stdDevBackground = totalStdDevSum / count;

    // Resize grid background map to full image resolution using bilinear interpolation
    cv::resize(gridBg, result.background, floatImg.size(), 0, 0, cv::INTER_LINEAR);
  }

  // Subtract background and floor negative values to zero
  cv::subtract(floatImg, result.background, result.subtracted);
  cv::threshold(result.subtracted, result.subtracted, 0, 0, cv::THRESH_TOZERO);

  return result;
}

} // namespace astroutils::stars

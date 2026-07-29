#pragma once

#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/optim.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

class Image : protected cv::Mat {
public:
};

////////////////////////////////////////
// Conversion //////////////////////////
////////////////////////////////////////

inline cv::Mat convert(const cv::Mat &image, int type = CV_8U,
                       double alpha = 1, double beta = 0) {
  cv::Mat result;
  image.convertTo(result, type, alpha, beta);
  return result;
}

inline cv::Mat color(const cv::Mat &image, int code) {
  cv::Mat result;
  cv::cvtColor(image, result, code);
  return result;
}

inline cv::Mat normalize(const cv::Mat &image) {
  cv::Mat result;
  cv::normalize(image, result, 0, 255,
    cv::NORM_MINMAX, CV_8U);
  return result;
}

////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////

inline cv::Mat background(const cv::Mat &image) {
  cv::Mat image32f;
  image.convertTo(image32f, CV_32F);

  int kernelRadius = 31; // Must be larger than the largest image element
  cv::Size kernelSize(kernelRadius, kernelRadius);
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, kernelSize);

  cv::Mat background;
  cv::morphologyEx(image32f, background, cv::MORPH_CLOSE, kernel);

  cv::Mat subtracted = image32f - background;
  cv::threshold(subtracted, subtracted, 0, 0, cv::THRESH_TOZERO);

  cv::Mat image8u;
  cv::normalize(subtracted, image8u, 0, 255, cv::NORM_MINMAX, CV_8U);

  return image8u;
}

inline cv::Mat clahe(const cv::Mat &image) {
  const double clipLimit = 20.0; // The higher - the brighter
  // 8x8 or 16x16 local regions
  const cv::Size tileGridSize(8, 8);

  cv::Mat result;
  auto clahe = cv::createCLAHE(clipLimit, tileGridSize);
  clahe->apply(image, result);

  return result;
}

inline cv::Mat grayscale(const cv::Mat &image) {
  cv::Mat result;
  cv::cvtColor(image, result, cv::COLOR_BGR2GRAY);
  return result;
}

inline cv::Mat lightness(const cv::Mat &image) {
  cv::Mat lab;
  cv::cvtColor(image, lab, cv::COLOR_BGR2Lab);

  std::vector<cv::Mat> channels;
  cv::split(lab, channels);

  return channels[0]; // L
}

inline cv::Mat stretchAsinh(const cv::Mat &image, float factor = 100) {
  cv::Mat image32f = convert(image, CV_32F);

  // Subtract minimal value
  double min, max;
  cv::minMaxLoc(image32f, &min, &max);
  image32f -= min;

  // Stretch
  image32f.forEach<float>([factor](float &pixel, const int *position) {
    pixel = std::asinh(factor * pixel) / std::asinh(factor);
  });

  return convert(image32f, CV_8U/* , 255 */);
}
#pragma once

#include "../utils/colors.h"
#include <cmath>
#include <format>
#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/check.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/optim.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/video/background_segm.hpp>
#include <print>
#include <string>
#include <vector>

class M : public cv::Mat {
  public:
    M() = default;
    M(const cv::Mat &image) : cv::Mat(image) {}
  };

////////////////////////////////////////

inline void printInfo(const std::string &title, const cv::Mat &image) {
  auto type = cv::typeToString(image.type());
  std::println("{}: {} {}x{}", bold(title), type, image.cols, image.rows);
}

inline std::string printInfo(const cv::Mat &image) {
  auto type = cv::typeToString(image.type());
  return std::format("{} {}x{}", type, image.cols, image.rows);
}

////////////////////////////////////////
// Conversion //////////////////////////
////////////////////////////////////////

inline cv::Mat convert(const cv::Mat &image, int type = CV_8U,
                       double alpha = 1, double beta = 0) {
  cv::Mat result;
  image.convertTo(result, type, alpha, beta);
  return result;
}

inline cv::Mat color(const cv::Mat &image, int code = cv::COLOR_BGR2GRAY) {
  cv::Mat result;
  cv::cvtColor(image, result, code);
  return result;
}

inline cv::Mat normalize(const cv::Mat &image,
                         int norm = cv::NORM_MINMAX,
                         int type = CV_8U) {
  cv::Mat result;
  cv::normalize(image, result, 0, 255,
    norm, type);
  return result;
}

inline cv::Mat resize(const cv::Mat &image, int width, int height) {
  cv::Mat result;
  cv::Size size(width, height);
  cv::resize(image, result, size);
  return result;
}

inline std::vector<cv::Mat> split(const cv::Mat &image) {
  std::vector<cv::Mat> channels;
  cv::split(image, channels);
  return channels;
}

inline cv::Mat merge(const std::vector<cv::Mat> &channels) {
  cv::Mat result;
  cv::merge(channels, result);
  return result;
}

////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////

inline cv::Mat stretchClahe(const cv::Mat &image, double clipLimit = 50.0, int tileSize = 8) {
  cv::Mat result;
  auto clahe = cv::createCLAHE(clipLimit, cv::Size(tileSize, tileSize));
  clahe->apply(image, result);
  return result;
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

  return normalize(image32f, cv::NORM_MINMAX, CV_32S);
}

struct MagicOptions {
  enum Type {
    CLAHE,
    Asinh
  };
  Type type = Type::CLAHE;
  double claheClipLimit = 10;
  int claheTileSize = 8;
  float asinhFactor = 100;
  int denoiseH = 5;
};

inline cv::Mat magic(const cv::Mat &image, const MagicOptions &options = {}) {
  cv::cvtColor(image, image, cv::COLOR_BGR2Lab);

  std::vector<cv::Mat> channels;
  cv::split(image, channels);

  if (options.type == MagicOptions::CLAHE) {
    channels[0] = stretchClahe(channels[0], 
      options.claheClipLimit,
      options.claheTileSize);
  } else {
    channels[0] = stretchAsinh(channels[0], options.asinhFactor);
    channels[0] = convert(channels[0], CV_8U);
  }

  if (options.denoiseH > 0) {
    cv::fastNlMeansDenoising(channels[0], channels[0],
      options.denoiseH, 7, 21);
  }

  cv::merge(channels, image);
  cv::cvtColor(image, image, cv::COLOR_Lab2BGR);

  return image;
}
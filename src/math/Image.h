#pragma once


#include "ImageStretcher.h"
#include "SharpnessEstimator.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

enum ImageSharpnessMethod {
  Gaussian,
  Laplacian
};

class Image : public cv::Mat {
public:
  Image() = default;
  Image(const cv::Mat &image) : cv::Mat(image) {}

  static std::vector<Image> toImageVectors(const std::vector<cv::Mat> &channels) {
    std::vector<Image> result;
    result.reserve(channels.size());

    for (const auto &channel : channels) {
      result.push_back(std::move(channel));
    }

    return result;
  }

  static std::vector<cv::Mat> toOpenCvMatVectors(const std::vector<Image> &channels) {
    std::vector<cv::Mat> result;
    result.reserve(channels.size());

    for (const auto &channel : channels) {
      result.push_back(std::move(channel));
    }

    return result;
  }

  static Image merge(const std::vector<Image> &channels) {
    Image result;
    cv::merge(toOpenCvMatVectors(channels), result);
    return result;
  }

  //////////////////////////////////////

  std::vector<Image> split() {
    std::vector<cv::Mat> channels;
    cv::split(*this, channels);
    return toImageVectors(channels);
  }

  Image clone() {
    return Image(cv::Mat::clone());
  }

  //////////////////////////////////////

  std::string getInfo() {
    auto info = cv::typeToString(type());
    return std::format("{} {}x{}", info, cols, rows);
  }

  double getSharpness(ImageSharpnessMethod method) {
    switch (method) {
    case Gaussian:
      return SharpnessEstimatorGaussian().getSharpness(*this);
    default:
      return SharpnessEstimatorLaplacian().getSharpness(*this);
    }
  }

  //////////////////////////////////////

  Image& convertTo(int type = CV_8U, double alpha = 1, double beta = 0) {
    cv::Mat::convertTo(*this, type, alpha, beta);
    return *this;
  }

  Image& cvtColor(int code = cv::COLOR_BGR2GRAY) {
    cv::cvtColor(*this, *this, code);
    return *this;
  }

  Image& normalize(int norm = cv::NORM_MINMAX,
    double alpha = 1, double beta = 0, int type = CV_8U) {
    cv::normalize(*this, *this, alpha, beta, norm, type);
    return *this;
  }

  Image& resize(int width, int height) {
    cv::Size size(width, height);
    cv::resize(*this, *this, size);
    return *this;
  }

  //////////////////////////////////////

  Image& preview(const std::string &title) {
    cv::imshow(title, *this);
    cv::waitKey();
    cv::destroyAllWindows();
    return *this;
  }

  //////////////////////////////////////

  Image stretch(const ImageStretcherOptions &options = {}) {
    ImageStretcher stretcher(*this);
    auto q = stretcher.stretch(options);
    return Image(q);
  }
};

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <cmath>
#include <vector>

struct ImageStretcherOptions {
  enum class Type { CLAHE, Asinh };

  Type type = Type::CLAHE;
  double claheClipLimit = 10;
  int claheTileSize = 8;
  float asinhFactor = 10;
  int denoiseH = 0;
};

class ImageStretcher {
  cv::Mat image;

public:
  ImageStretcher(const cv::Mat &image) : image(image) {}

  cv::Mat stretch(const ImageStretcherOptions &options = {}) {
    cv::cvtColor(image, image, cv::COLOR_BGR2Lab);

    auto channels = split(image);
    auto lightness = getLightness(channels[0]);

    if (options.type == ImageStretcherOptions::Type::CLAHE) {
      stretchClahe( channels[0],
        options.claheClipLimit,
        options.claheTileSize);
    } else {
      stretchAsinh(channels[0],
        options.asinhFactor);
    }

    scaleChroma(channels, lightness);

    if (options.denoiseH > 0) {
      cv::fastNlMeansDenoising(channels[0], channels[0],
        options.denoiseH, 7, 21);
    }

    cv::merge(channels, image);
    cv::cvtColor(image, image, cv::COLOR_Lab2BGR);

    return image;
  }

private:
  static std::vector<cv::Mat> split(const cv::Mat &image) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    return channels;
  }

  static cv::Mat getLightness(const cv::Mat &lightness) {
    cv::Mat result = lightness.clone();
    result.convertTo(result, CV_32F);
    cv::max(result, 1.0, result);
    return result;
  }

  static void stretchClahe(cv::Mat &image, double clipLimit = 50.0, int tileSize = 8) {
    auto clahe = cv::createCLAHE(clipLimit, cv::Size(tileSize, tileSize));
    clahe->apply(image, image);
  }

  static void stretchAsinh(cv::Mat &image, float factor = 100) {
    image.convertTo(image, CV_32F);

    double min, max;
    cv::minMaxLoc(image, &min, &max);
    image -= min;

    image.forEach<float>([factor](float &pixel, const int *position) {
      pixel = std::asinh(factor * pixel) / std::asinh(factor);
    });

    cv::normalize(image, image, 0, 255,
      cv::NORM_MINMAX, CV_32F);
  }

  static void scaleChroma(std::vector<cv::Mat> &channels, const cv::Mat &lightness) {
    for (int i = 0; i < 3; ++i) {
      channels[i].convertTo(channels[i], CV_32F);
      if (i > 0) channels[i] -= 128.0;
    }

    cv::Mat gain;
    cv::divide(channels[0], lightness, gain);
    cv::multiply(channels[1], gain, channels[1]);
    cv::multiply(channels[2], gain, channels[2]);

    for (int i = 0; i < 3; ++i) {
      if (i > 0) channels[i] += 128.0;
      channels[i].convertTo(channels[i], CV_8U);
    }
  }
};
#pragma once

#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <print>
#include <vector>

struct ImageBackgroundExtractorOptions {
  double structure = 81;
  double blur = 81;
};

struct ImageBackgroundResult {
  cv::Mat corrected {};
  cv::Mat background {};
};

class ImageBackgroundExtractor {
  cv::Mat image;

public:
  ImageBackgroundExtractor(const cv::Mat &image) : image(std::move(image)) {}

  ImageBackgroundResult extract(const ImageBackgroundExtractorOptions &options = {}) {
    std::println("extract()");

    auto divider = image.depth() == CV_16U 
      ? std::pow(2, 16) - 1
      : std::pow(2, 8) - 1;

    // Convert to float [0 to 1] range
    image.convertTo(image, CV_32F, 1 / divider);

    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    
    cv::Mat background;
    std::vector<cv::Mat> backgrounds(channels.size());

    for (int i = 0; i < channels.size(); ++i) {
      // Extract background
      cv::Size size = getSquare(options.structure);
      cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, size);
      cv::morphologyEx(channels[i], backgrounds[i], cv::MORPH_OPEN, kernel);

      // Blur background
      cv::Size blurSize(cv::Size(options.blur, options.blur));
      cv::GaussianBlur(backgrounds[i], backgrounds[i], blurSize, 0);

      // Divide background out
      double mean = getMean(backgrounds[i]);
      channels[i] = channels[i] / backgrounds[i] * mean;

      // Clip to [0; 1] range
      cv::threshold(channels[i], channels[i], 1, 1, cv::THRESH_TRUNC);
      cv::threshold(channels[i], channels[i], 0, 0, cv::THRESH_TOZERO);
    }

    cv::merge(channels, image);
    cv::merge(backgrounds, background);

    return {image, background};
  }

private:
  cv::Size getSquare(int size) {
    return cv::Size(size, size);
  }

  double getMean(const cv::Mat &image) {
    cv::Scalar means = cv::mean(image);

    double sum = 0;
    for (int i = 0; i < image.channels(); ++i) {
      sum += means[i];
    }

    return sum / image.channels();
  }
};
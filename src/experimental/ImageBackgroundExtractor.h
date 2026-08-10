#pragma once

#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
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
      backgrounds[i] = extract(channels[i], options.structure);
      backgrounds[i] = blur(backgrounds[i], options.blur);

      channels[i] = apply(channels[i], background);
      channels[i] = clip(channels[i]);
    }

    cv::merge(channels, image);
    cv::merge(backgrounds, background);

    return {image, background};
  }

private:
  cv::Mat extract(const cv::Mat &image, int structure) {
    cv::Mat background;
    cv::Size size = getSize(structure);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, size);
    cv::morphologyEx(image, background, cv::MORPH_OPEN, kernel);

    return background;
  }

  cv::Mat blur(const cv::Mat &image, int blur) {
    cv::Size blurSize(cv::Size(blur, blur));
    cv::GaussianBlur(image, image, blurSize, 0);
    return image;
  }

  cv::Mat apply(const cv::Mat &image, const cv::Mat &background) {
    double mean = getMean(background);
    return image / background * mean;
  }

  cv::Mat clip(const cv::Mat &image) {
    cv::threshold(image, image, 1, 1, cv::THRESH_TRUNC);
    cv::threshold(image, image, 0, 0, cv::THRESH_TOZERO);
    return image;
  }

  cv::Size getSize(int size) {
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
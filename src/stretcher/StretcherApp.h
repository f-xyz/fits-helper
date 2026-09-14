#pragma once

#include "StretcherConfig.h"
#include <Logger.hpp>
#include <image/image.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

using utils::image::ImageStretcher;
using utils::logging::Logger;

class StretcherApp : StretcherConfig {
  Logger &logger;

public:
  StretcherApp(StretcherConfig &config, Logger &logger)
      : StretcherConfig(config), logger(logger) {}

  void stretch() {
    const cv::Mat image = utils::image::read(file);
    const std::string info = utils::image::info(image);
    logger.info("Image info: {}", info);

    const cv::Size size(1280, 960);
    cv::resize(image, image, size);

    utils::image::ImageStretcher stretcher({
      .types = {stretchTypes},
      .claheClipLimit = claheClipLimit,
      .claheTileSize = claheTileSize,
      .asinhFactor = asinhFactor,
      .histogramTopBins = 10,
      .denoiseH = denoise
    });

    const cv::Mat stretched = stretcher.stretch(image);
    utils::image::show(stretched);
  }
};
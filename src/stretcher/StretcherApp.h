#pragma once

#include "Logger.hpp"
#include "StretcherConfig.h"
#include "image/image.hpp"

using utils::image::ImageStretcher;
using utils::logging::Logger;

class StretcherApp : StretcherConfig {
  Logger &logger;

public:
  explicit StretcherApp(StretcherConfig &config, Logger &logger)
      : StretcherConfig(config), logger(logger) {}

  void stretch() {
    cv::Mat image = utils::image::read(file);

    std::string info = utils::image::info(image);
    logger.info("Image info: {}", info);

    cv::Size size(1280, 960);
    cv::resize(image, image, size);

    utils::image::ImageStretcher stretcher({
      .types = {stretchTypes},
      .claheClipLimit = claheClipLimit,
      .claheTileSize = claheTileSize,
      .asinhFactor = asinhFactor,
      .histogramTopBins = 10,
      .denoiseH = denoise
    });

    cv::Mat stretched = stretcher.stretch(image.clone());
    utils::image::show(stretched);
  }
};
#pragma once

#include "StretcherConfig.h"
#include <astroutils/Logger.hpp>
#include <astroutils/image/image.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

using astroutils::image::ImageStretcher;
using astroutils::logging::Logger;

class StretcherApp : StretcherConfig {
  Logger &logger;

public:
  StretcherApp(StretcherConfig &config, Logger &logger)
      : StretcherConfig(config), logger(logger) {}

  void stretch() {
    const cv::Mat image = astroutils::image::read(file);
    const std::string info = astroutils::image::info(image);
    logger.info("Image info: {}", info);

    const cv::Size size(1280, 960);
    cv::Mat resized;
    cv::resize(image, resized, size);

    astroutils::image::ImageStretcher stretcher({
      .types = {stretchTypes},
      .claheClipLimit = claheClipLimit,
      .claheTileSize = claheTileSize,
      .asinhFactor = asinhFactor,
      .histogramTopBins = 10,
      .denoiseH = denoise
    });

    const cv::Mat stretched = stretcher.stretch(resized);
    astroutils::image::show(stretched);
  }
};

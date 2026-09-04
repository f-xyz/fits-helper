#pragma once

#include "../Config.h"
#include "Logger.hpp"
#include "image/image.hpp"

using namespace utils;
using namespace logging;

class StretcherApp : Config::StretcherConfig {
  Logger &logger;

public:
  explicit StretcherApp(Config &config, Logger &logger) : logger(logger) {
    file = config.stretcher.file;
    stretchTypes = config.stretcher.stretchTypes;
    claheClipLimit = config.stretcher.claheClipLimit;
    claheTileSize = config.stretcher.claheTileSize;
    asinhFactor = config.stretcher.asinhFactor;
    denoise = config.stretcher.denoise;
  }

  void stretch() {
    cv::Mat image = image::read(file);

    std::string info = image::info(image);
    logger.info("image: {}", info);

    cv::Size size(1280, 960);
    cv::resize(image, image, size);

    ImageStretcher stretcher({.types = {stretchTypes},
                              .claheClipLimit = claheClipLimit,
                              .claheTileSize = claheTileSize,
                              .asinhFactor = asinhFactor,
                              .histogramTopBins = 10,
                              .denoiseH = denoise});

    cv::Mat stretched = stretcher.stretch(image.clone());
    image::show(stretched);
  }
};
#pragma once

#include "Config.h"
#include "math/Image.h"
#include "utils/spark.h"
#include "fits/FitsReader.h"
#include "math/SharpnessEstimator.h"
#include <opencv2/imgcodecs.hpp>
#include <ranges>
#include <print>

struct Item {
  std::string file;
  double sharpness = 0;
};

class App {
  Config &config;
  SharpnessEstimator &estimator;

public:
  std::vector<Item> results;

  explicit App(Config &config, SharpnessEstimator &estimator)
    : config(config), estimator(estimator) {}

  void estimate() {
    for (int i = 0; i < config.files.size(); ++i) {
      auto file = std::filesystem::path(config.files[i]);
      auto image = readImage(file);

      auto roi = image({
        image.cols / 4,
        image.rows / 4,
        image.cols / 2,
        image.rows / 2
      });

      auto method = ImageSharpnessMethod::Gaussian;
      auto sharpness = Image(roi).getSharpness(method);
      results.push_back({ file, sharpness });

      std::println("#{}/{}", i + 1, config.files.size());
      std::println("Image: {}", file.filename().string());
      std::println("Sharpness: {}", sharpness);
      std::println("");
    }
  }

  std::vector<Item> percentiles() {
    std::ranges::sort(results, std::ranges::greater {}, &Item::sharpness);

    auto range = std::ranges::minmax(results, {}, &Item::sharpness);
    auto min = range.min.sharpness;
    auto max = range.max.sharpness;
    auto n = results.size();

    for (int i = 0; i < n; ++i) {
      auto item = results[i];
      auto dotPos = std::max<int>(0, item.file.find_last_of('.') - 4);
      auto name = item.file.substr(dotPos);
      auto percentile = n > 1 ? 1 - static_cast<double>(i) / (n - 1) : 0.5;

      auto color = config.type == Type::Best
        ? percentile >= config.percentile ? 0x00FF00 : 0x888888
        : percentile < config.percentile ? 0xFF0000 : 0x888888;

      auto line = std::format("{:<10}: {:.2f} ({:.2f}%)",
        name, item.sharpness, percentile);

      std::println("{}", rgb(line, color));
    }

    return {};
  }

  void printSpark() {
    auto values = results
      | std::views::transform(&Item::sharpness)
      | std::ranges::to<std::vector<double>>();

    std::println("{}\n", spark(values));
  }

private:
  Image readImage(const std::string &file) {
    auto ext = std::filesystem::path(file).extension().string();
    auto isFits = ext == ".fit" || ext == ".fits";
    return Image(isFits ? FitsReader().read(file) : cv::imread(file));
  }
};
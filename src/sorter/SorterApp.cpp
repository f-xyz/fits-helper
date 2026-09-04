#include "SorterApp.h"
#include "cli/spark.hpp"
#include "image/image.hpp"

void SorterApp::analyzeFiles() {
  logger.header("Processing files...\n");

  for (std::size_t i = 0; i < files.size(); ++i) {
    const auto file = files[i];
    const auto image = utils::image::read(file);

    cv::Rect roi {
      image.cols / 2 - image.cols / (SorterConfig::roi * 2),
      image.rows / 2 - image.rows / (SorterConfig::roi * 2),
      image.cols / SorterConfig::roi,
      image.rows / SorterConfig::roi
    };

    auto sharpness = estimator.getSharpness(image(roi));
    results.push_back({file, sharpness});

    logger.info("#{}/{}", i + 1, files.size());
    logger.info("Image: {}", file.filename().string());
    logger.info("Sharpness: {}\n", sharpness);
  }

  printSpark();
}

void SorterApp::processFiles(bool moveFiles) {
  logger.header("Computing percentiles...\n");

  if (moveFiles) {
    std::filesystem::create_directory(destination);
  }

  std::ranges::sort(results, std::ranges::greater {}, &Item::sharpness);

  auto n = results.size();
  for (std::size_t i = 0; i < n; ++i) {
    auto item = results[i];
    auto percentile = n > 1
      ? 1 - static_cast<double>(i) / (n - 1)
      : 0.5;

    auto isClipped = select == Config::Select::Better
      ? percentile >= SorterConfig::percentile
      : percentile <= SorterConfig::percentile;

    printReportLine(item, isClipped, percentile);

    if (moveFiles && isClipped) {
      std::filesystem::rename(item.file, destination / item.file.filename());
    }
  }
}

void SorterApp::printReportLine(const Item &item, bool isClipped, double percentile) {
  const auto name = item.file.string();
  const auto dotPos = std::max<int>(0, name.find_last_of('.') - 4);
  const auto alias = name.substr(dotPos);

  const auto color = select == Config::Select::Better
    ? isClipped ? 0x00FF00 : 0x888888
    : isClipped ? 0xFF0000 : 0x888888;

  auto line = std::format("{:<8}: {:.2f} ({:.2f}%) -> {}",
    alias, item.sharpness, percentile, isClipped ? "move" : "skip");

  logger.info("{}", utils::cli::rgb(line, color));
}

void SorterApp::printSpark() {
  const auto values = results | std::views::transform(&Item::sharpness);
  logger.info("{}\n", utils::cli::spark(values));
}
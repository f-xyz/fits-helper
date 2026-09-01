#include "Sorter.h"
#include "image/image.hpp"
#include "cli/spark.hpp"
#include <cstddef>

void Sorter::analyzeFiles() {
  logger.header("Processing files...\n");

  for (std::size_t i = 0; i < files.size(); ++i) {
    auto file = files[i];
    auto path = std::filesystem::path(file);
    auto image = utils::image::read(path);

    cv::Rect roi {
      image.cols / 2 - image.cols / (SorterConfig::roi * 2),
      image.rows / 2 - image.rows / (SorterConfig::roi * 2),
      image.cols / SorterConfig::roi,
      image.rows / SorterConfig::roi
    };

    auto sharpness = estimator.getSharpness(image(roi));
    results.push_back({path, sharpness});

    logger.info("#{}/{}", i + 1, files.size());
    logger.info("Image: {}", path.filename().string());
    logger.info("Sharpness: {}\n", sharpness);
  }
}

void Sorter::processFiles(bool moveFiles) {
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
      std::filesystem::path path(item.file);
      auto name = path.filename().string();
      std::filesystem::rename(item.file, destination + "/" + name);
    }
  }
}

void Sorter::printReportLine(const Item &item, bool isClipped, double percentile) {
  auto dotPos = std::max<int>(0, item.file.find_last_of('.') - 4);
  auto name = item.file.substr(dotPos);

  auto color = select == Config::Select::Better
    ? isClipped ? 0x00FF00 : 0x888888
    : isClipped ? 0xFF0000 : 0x888888;

  auto line = std::format("{:<8}: {:.2f} ({:.2f}%) -> {}",
    name, item.sharpness, percentile, isClipped ? "move" : "skip");

  logger.info("{}", utils::cli::rgb(line, color));
}

void Sorter::printSpark() {
  auto values = results | std::views::transform(&Item::sharpness);
  logger.info("{}\n", utils::cli::spark(values));
}
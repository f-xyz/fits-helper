#include "Sorter.h"
#include "fits/FitsReader.h"
#include "utils/spark.hpp"

cv::Mat Sorter::readImage(const std::string &file) {
  auto ext = std::filesystem::path(file).extension().string();
  auto isFits = ext == ".fit" || ext == ".fits";
  return isFits ? FitsReader().read(file) : cv::imread(file);
}

void Sorter::processFiles() {
  logger.header("Processing files...\n");

  for (int i = 0; i < files.size(); ++i) {
    auto file = files[i];
    auto path = std::filesystem::path(file);
    auto image = readImage(path);

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

void Sorter::computePercentiles(bool shouldMoveFiles) {
  logger.header("Computing percentiles...\n");

  if (shouldMoveFiles) {
    std::filesystem::create_directory(destination);
  }

  std::ranges::sort(results, std::ranges::greater {}, &Item::sharpness);

  auto n = results.size();
  auto range = std::ranges::minmax(results, {}, &Item::sharpness);
  auto min = range.min.sharpness;
  auto max = range.max.sharpness;

  for (int i = 0; i < n; ++i) {
    auto item = results[i];
    auto percentile = n > 1
      ? 1 - static_cast<double>(i) / (n - 1)
      : 0.5;

    auto isClipped = select == Config::Select::Better
      ? percentile >= SorterConfig::percentile
      : percentile <= SorterConfig::percentile;

    printReportLine(item, isClipped, percentile);

    if (shouldMoveFiles && isClipped) {
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

  logger.info("{}", rgb(line, color));
}

void Sorter::printSpark() {
  auto values = results
    | std::views::transform(&Item::sharpness)
    | std::ranges::to<std::vector<double>>();

  logger.info("{}\n", spark(values));
}
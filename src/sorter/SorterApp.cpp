#include "SorterApp.h"
#include "cli/colors.hpp"
#include "cli/spark.hpp"
#include "image/image.hpp"
#include "benchmarking/Timer.hpp"

using std::chrono::seconds;
using utils::benchmarking::Timer;

void SorterApp::analyzeFiles() {
  logger.header("Processing files...\n");
  Timer<seconds> timer;

  #pragma omp parallel for
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

    #pragma omp critical
    {
      const double percents = 100.0 * results.size() / files.size();
      logger.info("Progress: {:.1f}%", percents);
      logger.info("Image: {}", file.filename().string());
      logger.info("Sharpness: {}\n", sharpness);
    }
  }

  const auto seconds = timer.measure();
  logger.info("Duration: {}\n", seconds);

  printSpark();
}

void SorterApp::processFiles(bool moveFiles) {
  logger.header("Computing percentiles...\n");

  if (moveFiles) {
    std::filesystem::create_directory(destination);
  }

  std::ranges::sort(results, std::ranges::greater {}, &Item::sharpness);

  const auto n = results.size();
  for (std::size_t i = 0; i < n; ++i) {
    const auto item = results[i];
    const auto percentile = n > 1
      ? 1 - static_cast<double>(i) / (n - 1)
      : 0.5;

    const auto isClipped = select == SorterConfig::Select::Better
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

  const auto color = select == SorterConfig::Select::Better
    ? isClipped ? 0x00FF00 : 0x888888
    : isClipped ? 0xFF0000 : 0x888888;

  const auto line = std::format("{:<8}: {:.2f} ({:.2f}%) -> {}",
    alias, item.sharpness, percentile, isClipped ? "move" : "skip");

  logger.info("{}", utils::cli::rgb(line, color));
}

void SorterApp::printSpark() {
  const auto values = results | std::views::transform(&Item::sharpness);
  logger.info("{}\n", utils::cli::spark(values));
}
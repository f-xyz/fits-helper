#include "SorterApp.h"
#include <algorithm>
#include <astroutils/benchmarking/Timer.hpp>
#include <astroutils/cli/colors.hpp>
#include <astroutils/cli/spark.hpp>
#include <cstddef>
#include <filesystem>
#include <format>
#include <astroutils/fs.hpp>

using std::chrono::seconds;
using astroutils::benchmarking::Timer;
using astroutils::cli::rgb;
using astroutils::cli::spark;

std::vector<FileSharpness> SorterApp::analyzeFiles() const {
  logger.header("Analyzing files...\n");
  Timer<seconds> timer;

  const auto files = astroutils::fs::readDir(directory);
  const auto results = analyzer.analyzeFiles(files, roi);

  const auto seconds = timer.measure();
  logger.info("Finished in: {}\n", seconds);

  return results;
}

void SorterApp::processFiles(std::vector<FileSharpness> results, bool moveFiles) const {
  logger.header("Computing percentiles...\n");
  createOutputDirectory(moveFiles);

  std::ranges::sort(results, std::ranges::greater {}, &FileSharpness::sharpness);

  const auto n = results.size();
  for (std::size_t i = 0; i < n; ++i) {
    const auto& item = results[i];
    const auto percentile = n > 1
      ? 1 - static_cast<double>(i) / static_cast<double>((n - 1))
      : 0.5;

    const auto isClipped = select == SorterConfig::Select::Better
      ? percentile >= SorterConfig::percentile
      : percentile <= SorterConfig::percentile;

    printReportLine(item, isClipped, percentile);

    if (moveFiles && isClipped) {
      std::filesystem::rename(item.file, outDir / item.file.filename());
    }
  }
}

void SorterApp::createOutputDirectory(bool isNeeded) const {
  if (isNeeded) {
    std::filesystem::create_directory(outDir);
  }
}

void SorterApp::printReportLine(const FileSharpness &item, bool isClipped, double percentile) const {
  const auto name = item.file.string();
  const auto dotPos = name.find_last_of('.') - 4;
  const auto sliceStart = std::clamp<std::size_t>(dotPos, 0, name.size());
  const auto alias = name.substr(sliceStart);

  const auto color = select == SorterConfig::Select::Better
    ? isClipped ? 0x00FF00 : 0x888888
    : isClipped ? 0xFF0000 : 0x888888;

  const auto line = std::format("{:<8}: {:.2f} ({:.2f}%) -> {}",
    alias, item.sharpness, percentile, isClipped ? "move" : "skip");

  logger.info("{}", rgb(line, color));
}

void SorterApp::printSpark(const std::vector<FileSharpness> &results) const {
  const auto values = results | std::views::transform(&FileSharpness::sharpness);
  logger.info("{}\n", spark(values));
}
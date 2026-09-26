#include "SharpnessAnalyzer.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <execution>
#include <filesystem>
#include <astroutils/image/image.hpp>
#include <mutex>

std::vector<FileSharpness>
SharpnessAnalyzer::analyzeFiles(const std::vector<std::filesystem::path> &files,
                                int divider) const {

  std::mutex mutex;
  std::atomic<std::size_t> index = 0;
  std::vector<FileSharpness> results;
  results.reserve(files.size());

  std::for_each(std::execution::par, files.begin(), files.end(),
    [this, &results, &index, &files, &mutex, divider](const auto &file) {
      const auto modifiedAt = std::filesystem::last_write_time(file);
      const auto sharpness = getSharpness(file, divider);

      const auto nFiles = static_cast<double>(files.size());
      const auto nResults = index.fetch_add(1, std::memory_order_relaxed) + 1;
      const auto percents = 100.0 * static_cast<double>(nResults) / nFiles;

      {
        std::lock_guard lock(mutex);

        results.push_back({
          .file = file,
          .modifiedAt = modifiedAt,
          .sharpness = sharpness
        });

        log(file, sharpness, percents);
      }
    });

  std::ranges::sort(results,
    [](const FileSharpness &a, const FileSharpness &b) {
      return a.modifiedAt < b.modifiedAt;
    });

  return results;
}

double SharpnessAnalyzer::getSharpness(const std::filesystem::path &file,
                                       int divider) const {
  const auto image = astroutils::image::read(file.string());
  const auto roi = getRoi(image, divider);
  const auto sharpness = estimator.getSharpness(roi);

  return sharpness;
}

cv::Mat SharpnessAnalyzer::getRoi(const cv::Mat &image, int divider) const {
  cv::Rect roi {
    image.cols / 2 - image.cols / (divider * 2),
    image.rows / 2 - image.rows / (divider * 2),
    image.cols / divider,
    image.rows / divider
  };

  return image(roi);
}

double SharpnessAnalyzer::getProgress(const std::size_t nFiles,
                                      std::atomic<std::size_t> &index) const {

  const auto nResults = index.fetch_add(1, std::memory_order_relaxed) + 1;
  const auto percents =
      100.0 * static_cast<double>(nResults) / static_cast<double>(nFiles);

  return percents;
}

void SharpnessAnalyzer::log(const std::filesystem::path &file, double sharpness,
                            double percents) const {
  logger.info("Progress: {:.1f}%", percents);
  logger.info("Image: {}", file.filename().string());
  logger.info("Sharpness: {}\n", sharpness);
}
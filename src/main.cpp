#include "Config.h"
#include "utils/spark.h"
#include "fits/FitsReader.h"
#include "math/SharpnessEstimator.h"
#include "math/imageUtils.h"
#include <algorithm>
#include <filesystem>
#include <functional>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <print>
#include <ranges>
#include <stacktrace>
#include <csignal>
#include <string>
#include <vector>

void onSegfault(int signal) {
  std::println("Segmentation fault:");
  std::println("{}", std::stacktrace::current());
  std::signal(signal, SIG_DFL);
  std::raise(signal);
}

class App {
  Config &config;
  SharpnessEstimator &estimator;

public:
  explicit App(Config &config, SharpnessEstimator &estimator)
    : config(config), estimator(estimator) {}

  static cv::Mat readImage(const std::string &file) {
    auto ext = std::filesystem::path(file).extension().string();
    return ext == ".fit" || ext == ".fits"
      ? FitsReader().read(file)
      : cv::imread(file);
  }
};

int main(const int argc, const char **argv) {
  setenv("QT_QPA_PLATFORM", "xcb", 1); // Fixes QT windows on Wayland
  std::signal(SIGSEGV, onSegfault);
  std::println("FITS Helper\n");

  //////////////////////////////////////

  Config config;
  if (!config.parse(argc, argv)) {
    return 1;
  }

  //////////////////////////////////////

  struct Item {
    std::string file;
    std::string name;
    cv::Mat image {};
    double sharpness = 0;
  };

  // Estimation
  SharpnessEstimatorGaussian estimator;
  std::vector<Item> results;

  for (int i = 0; i < config.files.size(); ++i) {
    auto file = std::filesystem::path(config.files[i]);
    auto name = file.filename().string();
    auto image = App::readImage(file);

    auto roi = image({
      image.cols / 4,
      image.rows / 4,
      image.cols / 2,
      image.rows / 2
    });

    auto sharpness = estimator.getSharpness(roi);
    results.push_back({file, name, image, sharpness});

    std::println("#{}/{}", i + 1, config.files.size());
    std::println("Image: {}", name);
    std::println("Sharpness: {}", sharpness);
    std::println("");
  }

  // Spark
  auto values = results
    | std::views::transform(&Item::sharpness)
    | std::ranges::to<std::vector<double>>();
  std::println("{}\n", spark(values));

  // Sorted table
  std::println("-----------------------");
  std::ranges::sort(results, std::ranges::greater {}, &Item::sharpness);

  auto range = std::ranges::minmax(results, {}, &Item::sharpness);
  auto min = range.min.sharpness;
  auto max = range.max.sharpness;

  auto n = results.size();
  for (int i = 0; i < n; ++i) {
    auto item = results[i];
    auto dotPos = std::max<int>(0, item.name.find_last_of('.') - 4);
    auto name = item.name.substr(dotPos);

    auto scaledSharpness = (item.sharpness - min) / (max - min);
    auto percentile = n > 1 ? 1 - static_cast<double>(i) / (n - 1) : 0.5;

    struct Functor {
      int operator^(const double percentile) const {
        if (percentile >= 0.9) {
          return 0x00FF00;
        } else if (percentile <= 0.1) {
          return 0xFF0000;
        } else {
          return 0xAAAAAA;
        }
      }
    } f;

    auto color = f ^ percentile;

    std::println("{:<10}: {:.2f} ({:.2f}%)",
      rgb(name, color),
      item.sharpness,
      percentile);
  }

  //////////////////////////////////////
  // Previewing ////////////////////////
  //////////////////////////////////////

  auto best = results.front();
  auto preview = resize(best.image, 640, 480);

  if (preview.depth() != CV_8U) {
    preview.convertTo(preview, CV_8U, 1.0 / 256);
  }

  //////////////////////////////////////

  auto clahe = magic(preview.clone(), {
    .type = MagicOptions::CLAHE,
    .claheClipLimit = 20,
    .claheTileSize = 8,
    .asinhFactor = 20,
    .denoiseH = 5
  });

  //////////////////////////////////////

  cv::imshow("Original", preview);
  cv::waitKey();
  cv::destroyAllWindows();

  cv::imshow("Result", clahe);
  cv::waitKey();
  cv::destroyAllWindows();

  return 0;
}
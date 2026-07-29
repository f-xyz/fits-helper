#include "Config.h"
#include "utils/spark.h"
#include "fits/FitsReader.h"
#include "math/SharpnessEstimator.h"
#include "math/imageUtils.h"
#include <algorithm>
#include <filesystem>
#include <functional>
#include <opencv2/core/hal/interface.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <print>
#include <ranges>
#include <stacktrace>
#include <csignal>
#include <string>

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
  
  struct Image {
    std::string file;
    std::string name;
    cv::Mat image {};
    double sharpness = 0;
  };

  // Estimation
  SharpnessEstimatorGaussian estimator;
  std::vector<Image> results;

  for (int i = 0; i < config.files.size(); ++i) {
    auto file = std::filesystem::path(config.files[i]);
    auto name = file.filename().string();

    auto image = App::readImage(file);
    auto sharpness = estimator.getSharpness(image);
    results.push_back({file, name, image, sharpness});

    std::println("#{}/{}", i + 1, config.files.size());
    std::println("Image: {}", name);
    std::println("Sharpness: {}", sharpness);
    std::println("");
  }

  // Spark
  auto values = results
    | std::views::transform(&Image::sharpness)
    | std::ranges::to<std::vector<double>>();
  std::println("{}", spark(values));

  // Sorted table
  std::println("-----------------------");
  std::ranges::sort(results, std::ranges::greater {}, &Image::sharpness);

  for (auto &x : results) {
    std::println("{}: {}", x.name, x.sharpness);
  }

  // Previewing
  auto best = results.front();

  cv::Mat preview;
  cv::resize(best.image, preview, cv::Size(640, 480));
  
  // cv::Mat processed = grayscale(preview);
  // processed = clahe(processed);

  auto processed = lightness(normalize(preview));
  processed = stretchAsinh(processed);

  cv::imshow("Image", preview);
  cv::waitKey(0);
  cv::destroyAllWindows();

  cv::imshow("CLAHE", processed);
  cv::waitKey(0);
  cv::destroyAllWindows();

  return 0;
}
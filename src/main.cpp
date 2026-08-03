#include "Config.h"
#include "App.h"
#include "math/ImageStretcher.h"
#include "math/SharpnessEstimator.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/hal/interface.h>
#include <print>
#include <stacktrace>
#include <csignal>

void onSegfault(int signal) {
  std::println("Segmentation fault {}:", signal);
  std::println("{}", std::stacktrace::current());
  std::signal(signal, SIG_DFL);
  std::raise(signal);
}

int main(const int argc, const char **argv) {
  setenv("QT_QPA_PLATFORM", "xcb", 1); // Fixes QT windows on Wayland
  std::signal(SIGSEGV, onSegfault);
  std::println("FITS Helper\n");

  //////////////////////////////////////

  Config config(argc, argv);
  SharpnessEstimatorGaussian estimator;
  App app(config, estimator);

  // app.estimate();
  // app.printSpark();
  // app.percentiles();

  auto file = config.files.front();
  auto image = App::readImage(file).resize(1280, 960);

  auto clahe = image.clone().stretch({
    .type = ImageStretcherOptions::CLAHE,
    .claheClipLimit = 10,
    .claheTileSize = 8,
    .asinhFactor = 10,
    .denoiseH = 5
  });

  image.preview("Original");
  clahe.preview("CLAHE");

  return 0;
}
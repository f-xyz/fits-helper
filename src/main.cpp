#include "Config.h"
#include "App.h"
#include "math/ImageStretcher.h"
#include "utils/Logger.h"
#include "math/SharpnessEstimator.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
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

  Config config;
  config.parse(argc, argv, [&config](Config::Subcommand subcommand) {
    switch (subcommand) {
      case Config::Subcommand::Analyze:
      case Config::Subcommand::Move: {
        Logger logger(config.logFilePath);
        SharpnessEstimatorGaussian estimator;
        App app(config, logger, estimator);

        app.readFilesAndComputeSharpnesses();
        app.printSpark();
        app.computePercentiles(subcommand == Config::Subcommand::Move);
        break;
      }

      case Config::Subcommand::Clahe: {
        cv::Mat image = App::readImage(config.files.front());
        cv::Size size(1280, 960);
        cv::resize(image, image, size);

        cv::Mat stretched = ImageStretcher(image).stretch({
          .type = ImageStretcherOptions::Type::CLAHE,
          .claheClipLimit = 10,
          .claheTileSize = 8,
          .asinhFactor = 0.2,
          .denoiseH = 0
        });
        
        cv::imshow("CLAHE", stretched);
        cv::waitKey(0);
        cv::destroyAllWindows();
        break;
      }
    }
  });

  return 0;
}
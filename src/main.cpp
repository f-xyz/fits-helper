#include "Config.h"
#include "Sorter.h"
#include "lib/Logger.hpp"
#include "lib/image/ImageStretcher.hpp"
#include "lib/image/SharpnessEstimator.hpp"
#include "lib/image/image.hpp"

static void onSegfault(int signal) {
  std::println("Segmentation fault {}:", signal);
  std::println("{}", std::stacktrace::current());
  std::signal(signal, SIG_DFL);
  std::raise(signal);
}

int main(const int argc, const char **argv) {
  setenv("QT_QPA_PLATFORM", "xcb", 1); // Fixes QT windows on Wayland
  std::signal(SIGSEGV, onSegfault);
  std::println("{} v{}\n", NAME, VERSION);

  Config config;
  config.parse(argc, argv, [&config](Config::Subcommand subcommand) {
    switch (subcommand) {
      case Config::Subcommand::Analyze:
      case Config::Subcommand::Move: {
        Logger logger(config.common.logFilePath);
        SharpnessEstimatorGaussian estimator;
        Sorter app(config, logger, estimator);

        app.processFiles();
        app.printSpark();
        app.computePercentiles(subcommand == Config::Subcommand::Move);
        break;
      }

      case Config::Subcommand::Stretch: {
        std::string file = config.common.files.front();
        cv::Mat image = utils::image::read(file);
        cv::Size size(1280, 960);
        cv::resize(image, image, size);

        cv::Mat stretched = ImageStretcher(image).stretch({
          .type = config.stretcher.stretchType,
          .claheClipLimit = config.stretcher.claheClipLimit,
          .claheTileSize = config.stretcher.claheTileSize,
          .asinhFactor = config.stretcher.asinhFactor,
          .denoiseH = config.stretcher.denoise
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
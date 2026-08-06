#include "Config.h"
#include "App.h"
#include "utils/Logger.h"
#include "math/SharpnessEstimator.h"
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
      case Config::Subcommand::Analyze: {
        Logger logger(config.logFilePath);
        SharpnessEstimatorGaussian estimator;
        App app(config, logger, estimator);

        app.readFilesAndComputeSharpnesses();
        app.printSpark();
        app.computePercentiles(false);
        break;
      }

      case Config::Subcommand::Move: {
        Logger logger(config.logFilePath);
        SharpnessEstimatorGaussian estimator;
        App app(config, logger, estimator);

        app.readFilesAndComputeSharpnesses();
        app.printSpark();
        app.computePercentiles(true);
        break;
      }

      case Config::Subcommand::Clahe: {
        std::println("CLAHE");
        break;
      }
    }
  });

  return 0;
}
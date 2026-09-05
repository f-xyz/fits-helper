#include "Config.h"
#include "Logger.hpp"
#include "stacker/StackerApp.h"
#include "cli/colors.hpp"
#include "sorter/SorterApp.h"
#include "stretcher/StretcherApp.h"

static void onSegfault(int signal) {
  std::println("Segmentation fault {}:", signal);
  std::println("{}", std::stacktrace::current());
  std::signal(signal, SIG_DFL);
  std::raise(signal);
}

int main(const int argc, const char **argv) {
  setenv("QT_QPA_PLATFORM", "xcb", 1); // Fixes QT windows on Wayland
  std::signal(SIGSEGV, onSegfault);
  std::println("{} v{}\n", utils::cli::bold(NAME), utils::cli::bold(VERSION));

  Config config;
  Logger logger(config.common.logFilePath);

  config.parse(argc, argv, [&config, &logger](Config::Subcommand subcommand) {
    switch (subcommand) {
      case Config::Subcommand::Analyze:
      case Config::Subcommand::Move: {
        SharpnessEstimatorGaussian estimator;
        SorterApp app(config, logger, estimator);

        app.analyzeFiles();
        app.processFiles(subcommand == Config::Subcommand::Move);
        break;
      }

      case Config::Subcommand::Stretch: {
        StretcherApp app(config, logger);
        app.stretch();
        break;
      }

      case Config::Subcommand::Chop: {
        StackerApp app(config, logger);
        app.unchop();
        app.chop();
        app.stack();
        break;
      }

      case Config::Subcommand::Unchop: {
        StackerApp app(config, logger);
        app.unchop();
        break;
      }
    }
  });

  return 0;
}
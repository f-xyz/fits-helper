#include "Config.h"
#include "Subcommand.h"
#include "cli/colors.hpp"
#include "sorter/SorterApp.h"
#include "stacker/StackerApp.h"
#include "stretcher/StretcherApp.h"

using utils::image::SharpnessEstimatorGaussian;

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
  Logger logger(config.logFilePath);

  config.parse(argc, argv, [&config, &logger](Subcommand subcommand) {
    switch (subcommand) {
      case Subcommand::SorterAnalyze:
      case Subcommand::SorterSort: {
        SharpnessEstimatorGaussian estimator;
        SorterApp app(config.sorter, logger, estimator);

        app.analyzeFiles();
        app.processFiles(subcommand == Subcommand::SorterSort);
        break;
      }

      case Subcommand::Stretch: {
        StretcherApp app(config.stretcher, logger);
        app.stretch();
        break;
      }

      case Subcommand::Stack: {
        StackerApp app(config.stacker, logger);
        app.flatten();
        app.chop();
        app.stack();
        break;
      }

      case Subcommand::Unstack: {
        StackerApp app(config.stacker, logger);
        app.flatten();
        break;
      }
    }
  });

  return 0;
}
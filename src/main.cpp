#include "Config.h"
#include "Subcommand.h"
#include "image/SharpnessEstimator.hpp"
#include "sorter/SorterApp.h"
#include "stacker/StackerApp.h"
#include "stretcher/StretcherApp.h"
#include <cli/colors.hpp>
#include <utility>

using utils::image::SharpnessEstimatorGaussian;
using utils::logging::Logger;

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
  SharpnessEstimatorGaussian gaussian;
  SharpnessAnalyzer analyzer(logger, gaussian);

  config.parse(argc, argv, [&config, &logger, &analyzer](Subcommand subcommand) {
    switch (subcommand) {
      case Subcommand::SorterAnalyze:
      case Subcommand::SorterSort: {
        SorterApp sorter { config.sorter, logger, analyzer };
        auto results = sorter.analyzeFiles();
        auto moveFiles = subcommand == Subcommand::SorterSort;
        sorter.printSpark(results);
        sorter.processFiles(std::move(results), moveFiles);
        break;
      }

      case Subcommand::Stretch: {
        StretcherApp stretcher { config.stretcher, logger };
        stretcher.stretch();
        break;
      }

      case Subcommand::Stack: {
        StackerApp stacker { config.stacker, logger, analyzer };
        stacker.flatten();
        stacker.chop();
        stacker.stack();
        break;
      }

      case Subcommand::Unstack: {
        StackerApp stacker { config.stacker, logger, analyzer };
        stacker.flatten();
        break;
      }
    }
  });

  return 0;
}
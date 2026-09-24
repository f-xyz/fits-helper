#include "Config.h"
#include "Subcommand.h"
#include <astroutils/image/SharpnessEstimator.hpp>
#include "sorter/SorterApp.h"
#include "stacker/StackerApp.h"
#include "stretcher/StretcherApp.h"
#include <astroutils/TerminateHandler.hpp>
#include <astroutils/cli/colors.hpp>
#include <utility>

using utils::cli::bold;
using utils::image::SharpnessEstimatorGaussian;
using utils::logging::Logger;

int main(const int argc, const char **argv) {
  TerminateHandler::install();
  setenv("QT_QPA_PLATFORM", "xcb", 1); // Fixes QT windows on Wayland
  std::println("{} v{}\n", bold(NAME), bold(VERSION));

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
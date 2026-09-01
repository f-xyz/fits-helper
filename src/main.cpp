#include "Config.h"
#include "Logger.hpp"
#include "Sorter.h"
#include "cli/colors.hpp"
#include "exec.hpp"
#include "fs.hpp"
#include "image/image.hpp"
#include "string.hpp"
#include <ranges>
#include <regex>
#include <string>

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
        Sorter app(config, logger, estimator);

        app.processFiles();
        app.printSpark();
        app.computePercentiles(subcommand == Config::Subcommand::Move);
        break;
      }

      case Config::Subcommand::Stretch: {
        std::string file = config.common.files.front();
        cv::Mat image = utils::image::read(file);
        std::string info = utils::image::info(image);
        std::println("image: {}", info);

        cv::Size size(1280, 960);
        cv::resize(image, image, size);

        cv::Mat stretched = ImageStretcher({
          .types = {config.stretcher.stretchTypes},
          .claheClipLimit = config.stretcher.claheClipLimit,
          .claheTileSize = config.stretcher.claheTileSize,
          .asinhFactor = config.stretcher.asinhFactor,
          .histogramTopBins = 10,
          .denoiseH = config.stretcher.denoise
        }).stretch(image.clone());

        utils::image::show(stretched);
        break;
      }

      case Config::Subcommand::Chop: {
        const std::string templatePath = "scripts/stacker.ssf";
        const std::string tpl = utils::fs::readText(templatePath);
        const std::string script =
            utils::string::replace(R"(\{\$.+\})", tpl, "###");
        std::println("{}", script);

        const auto chunks = config.common.files
          | std::views::chunk(config.chopper.size);

        for (const auto &chunk : chunks) {
          std::println("------------");
          const std::filesystem::path first = chunk.front();
          const std::string dir = first.parent_path().string();
          std::println("{}", utils::cli::bold(dir));

          for (const auto &file : chunk) {
            std::println("{}", file);
          }

          std::println("");
        }

        auto q = utils::process::exec(
              "ping -c 4 8.8.8.8");
              // [](auto data) { std::print("{}", data); });
          std::println("{}", q.output);

        break;
      }
    }
  });

  return 0;
}
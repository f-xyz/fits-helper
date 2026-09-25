#include "StackerApp.h"
#include "ScriptGenerator.h"
#include "ScriptRunner.h"
#include <astroutils/benchmarking/Timer.hpp>
#include <cstddef>
#include <filesystem>
#include <ranges>
#include <string>

using astroutils::benchmarking::Timer;
using astroutils::cli::bold;
using astroutils::fs::writeFile;
using std::chrono::seconds;

void StackerApp::flatten() {
  logger.header("Flattenning {}\n", directory.string());

  for (const auto &dir : workspace.getDirs()) {
    logger.info("Moving files from: {}", bold(dir));
    workspace.moveFilesToParent(dir);

    logger.info("Removing directory: {}", bold(dir));
    std::filesystem::remove_all(dir);

    logger.info("");
  }
}

void StackerApp::chop() {
  logger.header("Analyzing files in {}\n", directory.string());

  Timer<seconds> timer;
  const auto chunks = workspace.getSortedChunks(chunkSize);
  const auto seconds = timer.measure();
  logger.success("Finished in: {}\n", seconds);

  if (std::ranges::empty(chunks)) {
    logger.error("The directory contains no files.\n");
    return;
  }

  logger.header("Chopping files in {}\n", directory.string());

  std::size_t index = 1;
  ScriptGenerator generator(*this);

  for (const auto &files : chunks) {
    const auto baseDir = files.front().parent_path();
    const auto chunkDir = baseDir / std::to_string(index);

    logger.info("Creating directory: {}", bold(chunkDir));
    std::filesystem::create_directory(chunkDir);

    logger.info("  Moving {} files", files.size());
    workspace.moveFilesToChild(chunkDir, files);

    logger.info("  Creating stacking scripts\n");
    const auto siril = generator.getStackerScript(chunkDir);
    const auto stack = generator.getShellScript(chunkDir);
    writeFile(chunkDir / "stacker.ssf", siril);
    writeFile(chunkDir / "stack.sh", stack);

    ++index;
  }
}

void StackerApp::stack() {
  logger.header("Stacking {}\n", directory.string());
  workspace.makeOutputDirectory();

  std::size_t index = 1;
  auto chunkDirs = workspace.getDirs();

  for (const auto &chunkDir : chunkDirs) {
    logger.info("Stacking images in: {} ({} of {})",
       bold(chunkDir), index, chunkDirs.size());

    ScriptRunner runner(chunkDir);
    const auto integrationPath = workspace.getOutputFileName(index);
    const auto result = runner.execute(integrationPath,
      [this](std::string_view message) {
        logger.info("  {}", message);
      });

    if (result.code == 0) {
      logger.success("  Finished in: {}\n", result.seconds);
    } else {
      logger.error("  Failed in: {}", result.seconds);
      logger.error("  Result code {}", result.code);
      logger.error("  See {} for details\n", (chunkDir / "stacker.log").string());
    }

    ++index;
  }
}
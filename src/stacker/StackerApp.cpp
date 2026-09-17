#include "StackerApp.h"
#include "ScriptGenerator.h"
#include "ScriptRunner.h"
#include <cstddef>
#include <filesystem>
#include <ranges>
#include <string>

using utils::cli::bold;

void StackerApp::flatten() {
  logger.header("Flattenning {}\n", directory.string());

  for (const auto &dir : workspace.getSortedDirs()) {
    logger.info("Moving files from: {}", bold(dir));
    workspace.moveFilesToParent(dir);

    logger.info("Removing directory: {}", bold(dir));
    std::filesystem::remove_all(dir);

    logger.info("");
  }
}

void StackerApp::chop() {
  logger.header("Chopping files in {}\n", directory.string());

  const auto chunks = workspace.getSortedChunks(chunkSize);
  if (std::ranges::empty(chunks)) {
    logger.error("The directory contains no files.\n");
    return;
  }

  std::size_t index = 1;
  ScriptGenerator generator(*this);

  for (const auto &chunk : chunks) {
    const auto baseDir = chunk.front().parent_path();
    const auto chunkDir = baseDir / std::to_string(index);

    logger.info("Creating directory: {}", bold(chunkDir));
    std::filesystem::create_directory(chunkDir);

    logger.info("  Creating stacking scripts");
    const auto siril = generator.getStackerScript(chunkDir);
    const auto stack = generator.getShellScript(chunkDir);
    utils::fs::writeFile(chunkDir / "stacker.ssf", siril);
    utils::fs::writeFile(chunkDir / "stack.sh", stack);

    logger.info("  Moving {} files\n", chunk.size());
    for (const auto &file : chunk) {
      const auto destination = chunkDir / file.filename();
      std::filesystem::rename(file, destination);
    }

    ++index;
  }
}

void StackerApp::stack() {
  logger.header("Stacking {}\n", directory.string());
  workspace.makeOutputDirectory();

  std::size_t index = 1;
  auto chunkDirs = workspace.getSortedDirs();

  for (const auto &chunkDir : chunkDirs) {
    logger.info("Stacking images in: {} ({} of {})",
       bold(chunkDir), index, chunkDirs.size());

    ScriptRunner runner(chunkDir);
    const auto integrationPath = workspace.getOutputFileName(index);
    const auto result = runner.execute(integrationPath);

    if (result.code == 0) {
      logger.info("  Finished in: {}\n", result.seconds);
    } else {
      logger.error("  Failed in: {}", result.seconds);
      logger.error("  Result code {}", result.code);
      logger.error("  See {} for details\n", (chunkDir / "stacker.log").string());
    }

    ++index;
  }
}
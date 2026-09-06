#include "StackerApp.h"
#include "ScriptRunner.h"
#include "ScriptGenerator.h"
#include "fs.hpp"
#include <cstdlib>
#include <filesystem>
#include <string>

void StackerApp::flatten() {
  auto dirs = fs::readDir(directory) | std::views::filter(isDirectory);

  for (const auto &dir : dirs) {
    const auto files = fs::readDir(dir);

    logger.info("Moving files from: {}", cli::bold(dir));
    for (const auto &file : files) {
      const auto ext = file.extension();
      const auto name = file.filename();

      if (ext == ".fit" || ext == ".fits") {
        const auto destination = directory / name;
        std::filesystem::rename(file, destination);
      }
    }

    logger.info("Removing directory: {}", cli::bold(dir));
    std::filesystem::remove_all(dir);

    logger.info("");
  }
}

void StackerApp::chop() {
  auto view = fs::readDir(directory)
    | std::views::filter(isRegularFile)
    | std::views::chunk(chunkSize);

  using Chunk = std::vector<std::vector<std::filesystem::path>>;
  const auto chunks = std::ranges::to<Chunk>(view);

  if (std::ranges::empty(chunks)) {
    logger.error("The provided directory contains no files.");
    return;
  }

  int index = 1;
  for (const auto &chunk : chunks) {
    const auto baseDir = chunk.front().parent_path();
    const auto chunkDir = baseDir / std::to_string(index);

    logger.info("Creating directory: {}", cli::bold(chunkDir));
    std::filesystem::create_directory(chunkDir);

    logger.info("  Creating stacking scripts");
    ScriptGenerator gen(*this);
    const auto siril = gen.getStackerScript(chunkDir);
    const auto stack = gen.getShellScript(chunkDir);
    fs::writeFile(chunkDir / "stacker.ssf", siril);
    fs::writeFile(chunkDir / "stack.sh", stack);

    logger.info("  Moving {} files", chunk.size());
    for (const auto &file : chunk) {
      const auto destination = chunkDir / file.filename();
      std::filesystem::rename(file, destination);
    }

    ++index;
    logger.info("");
  }
}

void StackerApp::stack() {
  auto chunkDirs = fs::readDir(directory);
  std::ranges::sort(chunkDirs , comparePaths);

  const auto masterDirName = directory.filename().string() + "-stacked";
  const auto masterDirPath = directory.parent_path() / masterDirName;
  std::filesystem::remove_all(masterDirPath);
  std::filesystem::create_directory(masterDirPath);

  int index = 1;
  for (const auto &chunkDir : chunkDirs) {
    logger.info("Stacking images in: {} ({} of {})",
       cli::bold(chunkDir), index, chunkDirs.size());

    const auto result = ScriptRunner(chunkDir).execute();
    if (result.code == 0) {
      logger.info("  Finished in {} s", result.seconds);
      logger.info("  Result code {} s", result.code);

      const auto integrationName = std::to_string(index) + ".fit";
      const auto integrationPath = masterDirPath / integrationName;
      std::filesystem::rename(result.integration, integrationPath);

      logger.info("");
      ++index;
      break; //////////////////////////////////////// !!!
    } else {
      logger.error("  Stacking has failed.");
      logger.error("  Code: {}", result.code);
      std::println("  See stacker.log");
      std::exit(1);
    }
  }
}
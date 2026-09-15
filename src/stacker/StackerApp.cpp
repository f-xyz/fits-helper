#include "StackerApp.h"
#include "ScriptGenerator.h"
#include "ScriptRunner.h"
#include <cstddef>
#include <filesystem>
#include <ranges>
#include <string>

void StackerApp::flatten() const {
  logger.header("Flattenning the directory...\n");

  auto dirs = fs::readDir(directory) | std::views::filter(isDirectory);

  for (const auto &dir : dirs) {
    const auto files = fs::readDir(dir);

    logger.info("Moving files from: {}", cli::bold(dir));
    for (const auto &file : files) {
      const auto ext = file.extension();
      if (ext == ".fit" || ext == ".fits") {
        std::filesystem::rename(file, directory / file.filename());
      }
    }

    logger.info("Removing directory: {}", cli::bold(dir));
    std::filesystem::remove_all(dir);

    logger.info("");
  }
}

void StackerApp::chop() const {
  logger.header("Analyzing image sharpnesses...\n");

  auto qwe = fs::readDir(directory) | std::views::filter(isRegularFile);
  auto files = std::ranges::to<std::vector<std::filesystem::path>>(qwe);

  auto results = analyzer.analyzeFiles(files, 2);
  std::ranges::sort(results, std::ranges::greater(), &FileSharpness::sharpness);

  logger.header("Chopping the directory...\n");

  auto view = results
    | std::views::transform(&FileSharpness::file)
    | std::views::chunk(chunkSize);

  using Chunk = std::vector<std::vector<std::filesystem::path>>;
  const auto chunks = std::ranges::to<Chunk>(view);

  if (std::ranges::empty(chunks)) {
    logger.error("The provided directory contains no files.");
    return;
  }

  std::size_t index = 1;
  for (const auto &chunk : chunks) {
    const auto baseDir = chunk.front().parent_path();
    const auto chunkDir = baseDir / std::to_string(index);

    logger.info("Creating directory: {}", cli::bold(chunkDir));
    std::filesystem::create_directory(chunkDir);

    logger.info("  Creating stacking scripts");
    ScriptGenerator generator(*this);
    const auto siril = generator.getStackerScript(chunkDir);
    const auto stack = generator.getShellScript(chunkDir);
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

void StackerApp::stack() const {
  logger.header("Stacking...\n");

  auto chunkDirs = fs::readDir(directory);
  std::ranges::sort(chunkDirs , comparePaths);

  const auto masterDirName = directory.filename().string() + "-stacked";
  const auto masterDirPath = directory.parent_path() / masterDirName;

  std::filesystem::remove_all(masterDirPath);
  std::filesystem::create_directory(masterDirPath);

  std::size_t index = 1;
  for (const auto &chunkDir : chunkDirs) {
    logger.info("Stacking images in: {} ({} of {})",
       cli::bold(chunkDir), index, chunkDirs.size());

    ScriptRunner runner(chunkDir);
    const auto integrationName = std::to_string(index) + ".fit";
    const auto integrationPath = masterDirPath / integrationName;
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
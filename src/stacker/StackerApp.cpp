#include "StackerApp.h"
#include "ScriptRunner.h"
#include "ScriptGenerator.h"
#include <filesystem>

void StackerApp::unchop() {
  std::filesystem::remove_all(directory / "masters");

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
    | std::views::chunk(size);

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
  auto dirs = fs::readDir(directory);
  std::ranges::sort(dirs , comparePaths);

  std::filesystem::create_directory(directory / "masters");

  int index = 1;
  for (const auto &dir : dirs) {
    logger.info("Processing directory: {} ({} of {})",
       cli::bold(dir), index, dirs.size());

    ScriptRunner(dir, index).execute();
    ++index;

    break; // !!!
  }
}
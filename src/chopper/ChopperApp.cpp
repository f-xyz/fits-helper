#include "ChopperApp.h"
#include "fs.hpp"
#include "string.hpp"
#include <filesystem>

void ChopperApp::chop() {
  auto view = files
    | std::views::filter(isRegularFile)
    | std::views::chunk(size);

  using Chunk = std::vector<std::vector<std::filesystem::path>>;
  const auto chunks = std::ranges::to<Chunk>(view);

  if (std::ranges::empty(chunks)) {
    logger.error("The provided directory contains no files.");
    return;
  }

  int iChunk = 0;
  for (const auto &chunk : chunks) {
    const auto baseDir = chunk.front().parent_path();
    const auto chunkDir = baseDir / std::to_string(iChunk + 1);

    logger.info("Creating directory: {}", cli::bold(chunkDir));
    std::filesystem::create_directory(chunkDir);

    logger.info("  Creating stacker script");
    const auto script = getStackerScript(chunkDir);
    fs::writeFile(chunkDir / "stacker.ssf", script);

    logger.info("  Moving {} files", chunk.size());
    for (const auto &file : chunk) {
      const auto destination = chunkDir / file.filename();
      std::filesystem::rename(file, destination);
    }

    ++iChunk;
    logger.info("");
  }
}

void ChopperApp::unchop() {
  auto dirs = fs::readDir(dir) | std::views::filter(isDirectory);

  if (std::ranges::empty(dirs)) {
    logger.error("The provided directory contains no directories.");
    return;
  }

  for (const auto &dir : dirs) {
    const auto files = fs::readDir(dir);

    logger.info("Moving files from: {}", cli::bold(dir));
    for (const auto &file : files) {
      const auto ext = file.extension();
      const auto name = file.filename();

      if (ext != ".ssf") {
        const auto destination = ChopperConfig::dir / name;
        std::filesystem::rename(file, destination);
      }
    }

    logger.info("Removing directory: {}", cli::bold(dir));
    std::filesystem::remove_all(dir);

    logger.info("");
  }
}

std::string ChopperApp::getStackerScript(const std::filesystem::path &chunkDir) {
  const std::string templatePath = "scripts/stacker.ssf";
  const std::string tpl = fs::readText(templatePath);
  const std::string script = string::replace(R"(\$\{.+\})",
      tpl, std::filesystem::absolute(chunkDir));

  return script;
}
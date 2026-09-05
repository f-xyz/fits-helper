#include "StackerApp.h"
#include "benchmark.hpp"
#include "fs.hpp"
#include "process.hpp"
#include "string.hpp"
#include <chrono>

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

  int iChunk = 0;
  for (const auto &chunk : chunks) {
    const auto baseDir = chunk.front().parent_path();
    const auto chunkDir = baseDir / std::to_string(iChunk + 1);

    logger.info("Creating directory: {}", cli::bold(chunkDir));
    std::filesystem::create_directory(chunkDir);

    logger.info("  Creating stacking scripts");
    const auto siril = getStackerScript(chunkDir);
    const auto stack = getShellScript(chunkDir);
    fs::writeFile(chunkDir / "stacker.ssf", siril);
    fs::writeFile(chunkDir / "stack.sh", stack);
    std::filesystem::copy("scripts/clean.sh", chunkDir / "clean.sh");

    logger.info("  Moving {} files", chunk.size());
    for (const auto &file : chunk) {
      const auto destination = chunkDir / file.filename();
      std::filesystem::rename(file, destination);
    }

    ++iChunk;
    logger.info("");
  }
}

void StackerApp::stack() {
  using namespace std::chrono_literals;

  auto dirs = fs::readDir(directory);
  std::ranges::sort(dirs , comparePaths);

  for (const auto &dir : dirs) {
    logger.info("Processing directory: {}", dir.string());

    const std::string command = "/usr/bin/bash " + (dir / "stack.sh").string();
    std::println("Command: {}", command);

    const auto seconds = utils::benchmark<std::chrono::seconds>([&command]() {
      const auto result = process::exec(command, [](const auto &x) { std::println("{}", x); });
      std::println("Code: {}", result.code);
    });

    std::println("Time: {} sec", seconds);
    break;
  }
}

void StackerApp::unchop() {
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

std::string StackerApp::getStackerScript(const std::filesystem::path &chunkDir) {
  const std::map<std::string, std::string> vars = {
    {"${PATH}", std::filesystem::canonical(chunkDir)},
    {"${CALIBRATION}", getCalibration()}
  };

  std::string script = fs::readText(sirilScript);

  for (const auto &[key, value] : vars) {
    script = string::replace_all(script, key, value);
  }

  return script;
}

std::string StackerApp::getShellScript(const std::filesystem::path &chunkDir) {
  const std::map<std::string, std::string> vars = {
    {"${PATH}", std::filesystem::canonical(chunkDir)}
  };

  std::string script = fs::readText(stackScript);

  for (const auto &[key, value] : vars) {
    script = string::replace_all(script, key, value);
  }

  return script;
}

std::string StackerApp::getCalibration() {
  const std::map<std::string, std::string> vars = {
    {"-bias", bias},
    {"-dark", dark},
    {"-flat", flat}
  };

  std::vector<std::string> result;

  for (const auto &[key, value] : vars) {
    if (!value.empty()) {
      std::string path = std::filesystem::canonical(value);
      std::string item = key + "=" + path;
      result.push_back(item);
    }
  }

  return string::join(result, " ");
}
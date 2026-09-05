#include "ChopperApp.h"
#include "fs.hpp"
#include "process.hpp"
#include "string.hpp"
#include <chrono>
#include <thread>

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

    logger.info("  Creating stacking scripts");
    const auto script = getStackerScript(chunkDir);
    fs::writeFile(chunkDir / "stacker.ssf", script);
    std::filesystem::copy("scripts/stack.sh", chunkDir / "stack.sh");
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

void ChopperApp::stack() {
  using namespace std::chrono_literals;

  const auto startTime = std::chrono::system_clock::now();
  // const auto result = process::exec("./stack.sh");
  std::this_thread::sleep_for(1s);
  const auto endTime = std::chrono::system_clock::now();
  const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
  std::println("seconds: {}", seconds);
}

void ChopperApp::unchop() {
  auto dirs = fs::readDir(unchopDir) | std::views::filter(isDirectory);

  for (const auto &dir : dirs) {
    const auto files = fs::readDir(dir);

    logger.info("Moving files from: {}", cli::bold(dir));
    for (const auto &file : files) {
      const auto ext = file.extension();
      const auto name = file.filename();

      if (ext == ".fit" || ext == ".fits") {
        const auto destination = unchopDir / name;
        std::filesystem::rename(file, destination);
      }
    }

    logger.info("Removing directory: {}", cli::bold(dir));
    std::filesystem::remove_all(dir);

    logger.info("");
  }
}

std::string ChopperApp::getStackerScript(const std::filesystem::path &chunkDir) {
  const std::map<std::string, std::string> vars = {
    {"${PATH}", std::filesystem::canonical(chunkDir)},
    {"${CALIBRATION}", getCalibration()}
  };

  std::string script = fs::readText(scriptTemplatePath);

  for (const auto &[key, value] : vars) {
    for (;;) {
      const auto index = script.find(key);
      if (index != std::string::npos) {
        script.replace(index, key.size(), value);
      } else {
        break;
      }
    }
  }

  return script;
}

std::string ChopperApp::getCalibration() {
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
#pragma once

#include "../SharpnessAnalyzer.h"
#include <fs.hpp>

using std::filesystem::path;
using Files = std::vector<std::filesystem::path>;
using Chunks = std::vector<std::vector<path>>;

class Workspace {
  const std::string outDirSuffix = "-stacked";

  SharpnessAnalyzer &analyzer;
  path directory;

public:
  explicit Workspace(SharpnessAnalyzer &analyzer, const path &directory)
      : analyzer(analyzer), directory(directory) {}

  ////////////////////////////////////////
  // Files ///////////////////////////////
  ////////////////////////////////////////

  Files getFiles() {
    auto contents = utils::fs::readDir(directory);
    auto files = contents | std::views::filter(isRegularFile);
    return std::ranges::to<Files>(files);
  }

  Chunks getSortedChunks(int chunkSize) {
    const auto files = getFiles();

    auto results = analyzer.analyzeFiles(files, 2);
    std::ranges::sort(results, std::ranges::greater(),
                      &FileSharpness::sharpness);

    auto view = results
      | std::views::transform(&FileSharpness::file)
      | std::views::chunk(chunkSize);

    return std::ranges::to<Chunks>(view);
  }

  void moveFilesToParent(const path &dir) {
    const auto files = utils::fs::readDir(dir);
    for (const auto &file : files) {
      const auto ext = file.extension();
      if (ext == ".fit" || ext == ".fits") {
        std::filesystem::rename(file, directory / file.filename());
      }
    }
  }

  ////////////////////////////////////////
  // Directories /////////////////////////
  ////////////////////////////////////////

  Files getDirs() {
    auto dirs = utils::fs::readDir(directory);
    return std::ranges::to<Files>(dirs | std::views::filter(isDirectory));
  }

  Files getSortedDirs() {
    auto dirs = getDirs();
    std::ranges::sort(dirs, comparePaths);
    return std::ranges::to<Files>(dirs);
  }

  ////////////////////////////////////////
  // Output //////////////////////////////
  ////////////////////////////////////////

  void makeOutputDirectory() {
    const auto outDirName = directory.filename().string() + outDirSuffix;
    const auto outDirPath = directory.parent_path() / outDirName;

    std::filesystem::remove_all(outDirPath);
    std::filesystem::create_directory(outDirPath);
  }

  path getOutputFileName(std::size_t index) {
    const auto outDirName = directory.filename().string() + outDirSuffix;
    const auto outDirPath = directory.parent_path() / outDirName;

    const auto integrationName = std::to_string(index) + ".fit";
    const auto integrationPath = outDirPath / integrationName;

    return integrationPath;
  }

private:
  static bool isRegularFile(const path &path) {
    return std::filesystem::is_regular_file(path);
  }

  static bool isDirectory(const path &path) {
    return std::filesystem::is_directory(path);
  }

  static bool comparePaths(const path &a, const path &b) {
    try {
      const auto an = std::stoi(a.filename().string());
      const auto bn = std::stoi(b.filename().string());
      return an < bn;
    } catch (const std::invalid_argument &) {
      return a.filename() < b.filename();
    }
  }
};
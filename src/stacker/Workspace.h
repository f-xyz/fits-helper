#pragma once

#include "../SharpnessAnalyzer.h"
#include <astroutils/fs.hpp>

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

  Files getFiles();
  Files getDirs();

  Chunks getSortedChunks(int chunkSize);

  void moveFilesToParent(const path &dir);
  void moveFilesToChild(const path &dir, const Files &files);

  ////////////////////////////////////////
  // Output //////////////////////////////
  ////////////////////////////////////////

  void makeOutputDirectory();
  path getOutputFileName(std::size_t index);

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
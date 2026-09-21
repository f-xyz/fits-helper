#include "Workspace.h"
#include <ranges>

////////////////////////////////////////
// Files ///////////////////////////////
////////////////////////////////////////

Files Workspace::getFiles() {
  auto contents = utils::fs::readDir(directory)
    | std::views::filter(isRegularFile);

  return std::ranges::to<Files>(contents);
}

Files Workspace::getDirs() {
  auto contents = utils::fs::readDir(directory)
    | std::views::filter(isDirectory);

  auto dirs = std::ranges::to<Files>(contents);
  std::ranges::sort(dirs, comparePaths);

  return dirs;
}

Chunks Workspace::getSortedChunks(int chunkSize) {
  const auto files = getFiles();

  auto results = analyzer.analyzeFiles(files, 2);
  std::ranges::sort(results, std::ranges::greater(),
    &FileSharpness::sharpness);

  auto view = results
    | std::views::transform(&FileSharpness::file)
    | std::views::chunk(chunkSize);

  return std::ranges::to<Chunks>(view);
}

void Workspace::moveFilesToParent(const path &dir) {
  for (const auto &file : utils::fs::readDir(dir)) {
    const auto ext = file.extension();
    if (ext == ".fit" || ext == ".fits") {
      std::filesystem::rename(file, directory / file.filename());
    }
  }
}

void Workspace::moveFilesToChild(const path &dir, const Files &files) {
  for (const auto &file : files) {
    const auto destination = dir / file.filename();
    std::filesystem::rename(file, destination);
  }
}

////////////////////////////////////////
// Output //////////////////////////////
////////////////////////////////////////

void Workspace::makeOutputDirectory() {
  const auto outDirName = directory.filename().string() + outDirSuffix;
  const auto outDirPath = directory.parent_path() / outDirName;

  std::filesystem::remove_all(outDirPath);
  std::filesystem::create_directory(outDirPath);
}

path Workspace::getOutputFileName(std::size_t index) {
  const auto outDirName = directory.filename().string() + outDirSuffix;
  const auto outDirPath = directory.parent_path() / outDirName;

  const auto integrationName = std::to_string(index) + ".fit";
  const auto integrationPath = outDirPath / integrationName;

  return integrationPath;
}
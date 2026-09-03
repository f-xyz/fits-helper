#include "ChopperApp.h"

void ChopperApp::chop() {
  auto view = files
    | std::views::filter(isRegularFile)
    | std::views::chunk(size);

  using Chunk = std::vector<std::vector<std::string>>;
  auto chunks = std::ranges::to<Chunk>(view);

  for (std::size_t iChunk = 0; iChunk < chunks.size(); ++iChunk) {
    const std::vector<std::string> chunk = chunks[iChunk];
    const std::string baseDir = getBaseDir(chunk.front());
    logger.header("Base directory: {}", cli::bold(baseDir));

    const std::string chunkDir = getChunkDir(baseDir, iChunk);
    std::filesystem::create_directory(chunkDir);
    logger.info("Created directory: {}", cli::bold(chunkDir));

    for (const std::string &file : chunk) {
      const std::filesystem::path path(file);
      const std::string source = path.string();
      const std::string destination = chunkDir + "/" + path.filename().string();
      logger.info("from: {}", source);
      logger.info("to:   {}", destination);
      std::filesystem::rename(file, destination);
    }

    logger.info("");
  }
}

void ChopperApp::unchop() {
  const auto dirs =  fs::readDir(dir);
  for (const auto &dir : dirs) {

    logger.info("{}", dir);
    for (const auto &file : fs::readDir(dir)) {
      std::filesystem::path path(file);
      const auto ext = path.extension().string();
      const auto name = path.filename().string();
      logger.info("  > {}", file);
      
      if (ext != "ssf") {
        std::filesystem::rename(file, ChopperConfig::dir + "/" + name);
      }
    }

    std::filesystem::remove_all(dir);
    logger.info("");
  }
}

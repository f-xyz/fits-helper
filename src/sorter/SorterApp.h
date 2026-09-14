#pragma once

#include "../SharpnessAnalyzer.h"
#include "SorterConfig.h"
#include <Logger.hpp>
#include <cstdlib>
#include <vector>

using utils::logging::Logger;

class SorterApp : SorterConfig {
  Logger &logger;
  SharpnessAnalyzer &analyzer;

public:
  SorterApp(const SorterConfig &config, Logger &logger, SharpnessAnalyzer &analyzer)
      : SorterConfig(config), logger(logger), analyzer(analyzer) {}

  std::vector<FileSharpness> analyzeFiles() const;
  void processFiles(std::vector<FileSharpness> results, bool moveFiles) const;
  void printSpark(const std::vector<FileSharpness> &results) const;

private:
  void createOutputDirectory(bool isNeeded) const;
  void printReportLine(const FileSharpness &item, bool isClipped,
                       double percentile) const;
};
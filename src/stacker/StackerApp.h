#pragma once

#include "../SharpnessAnalyzer.h"
#include "StackerConfig.h"
#include "Workspace.h"
#include <astroutils/Logger.hpp>

class StackerApp : StackerConfig {
  Logger &logger;
  Workspace workspace;

public:
  StackerApp(const StackerConfig &config, Logger &logger, SharpnessAnalyzer &analyzer) :
    StackerConfig(config),
    logger(logger),
    workspace(analyzer, directory) {}

  void flatten();
  void chop();
  void stack();
};
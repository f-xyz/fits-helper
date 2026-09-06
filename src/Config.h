#pragma once

#include "Subcommand.h"
#include "sorter/SorterConfig.h"
#include "stacker/StackerConfig.h"
#include "stretcher/StretcherConfig.h"

class Config {
public:
  std::string logFilePath = "fits-helper.log";
  SorterConfig sorter;
  StretcherConfig stretcher;
  StackerConfig stacker;

  int parse(const int argc, const char **argv,
            const std::function<void(Subcommand)> &callback);
};
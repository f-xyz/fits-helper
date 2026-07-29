#pragma once

#include <CLI11.hpp>
#include <string>
#include <vector>

class Config {
public:
  std::vector<std::string> files;
  unsigned int percentile = 10;

  bool parse(const int argc, const char **argv);
};
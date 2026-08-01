#pragma once

#include <CLI11.hpp>
#include <exception>
#include <string>
#include <vector>

enum Type {
  Best,
  Worst
};

class Config {
public:
  std::vector<std::string> files;
  double percentile = 0.1;
  Type type = Type::Best;

  explicit Config(const int argc, const char **argv) {
    if (!parse(argc, argv)) {
      std::terminate();
    }
  }

  bool parse(const int argc, const char **argv);
};
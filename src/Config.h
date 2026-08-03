#pragma once

#include <CLI11.hpp>
#include <exception>
#include <string>
#include <vector>

class Config {
public:

  enum class Select {
    Better,
    Worse
  };

  enum class Operation {
    View,
    Move
  };

  std::string logFilePath = "culler.log";
  Select select = Select::Better;
  double percentile = 0.1;
  int roi = 2;
  Operation operation = Operation::Move;
  std::string destination = "_bad";
  std::vector<std::string> files;

  explicit Config(const int argc, const char **argv) {
    if (!parse(argc, argv)) {
      std::terminate();
    }
  }

  bool parse(const int argc, const char **argv);
};
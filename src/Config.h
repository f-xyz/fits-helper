#pragma once

#include <CLI11.hpp>
#include <string>
#include <vector>

class Config {
public:
  enum class Subcommand { Analyze, Move, Clahe };
  enum class Select { Better, Worse };

  // Sharpness: Analyze / Move
  std::string logFilePath = "fits-helper.log";
  Select select = Select::Better;
  double percentile = 0.1;
  int roi = 2;
  std::vector<std::string> files;
  std::string destination;

  int parse(const int argc, const char **argv, const std::function<void(Subcommand)> &callback) {
    CLI::App app;
    app.require_subcommand(1);

    auto analyze = app.add_subcommand("analyze");
    addOptionSelect(analyze, true);
    addOptionPercentile(analyze, true);
    addOptionRoi(analyze, false);
    addOptionFiles(analyze, true);
    addOptionDestination(analyze, false);
    analyze->callback([&callback]() {
      callback(Subcommand::Analyze);
    });

    auto move = app.add_subcommand("move");
    addOptionSelect(move, true);
    addOptionPercentile(move, true);
    addOptionRoi(move, false);
    addOptionFiles(move, true);
    addOptionDestination(move, true);
    move->callback([&callback]() {
      callback(Subcommand::Move);
    });

    auto clahe = app.add_subcommand("clahe");
    addOptionFiles(clahe, true);
    clahe->callback([&callback]() {
      callback(Subcommand::Clahe);
    });

    try {
      app.parse(argc, argv);
      return 0;
    } catch (const CLI::ParseError &e) {
      return app.exit(e);
    }
  }

private:
  const std::map<std::string, Select> selectMap = {
    {"better", Select::Better},
    {"worse", Select::Worse}
  };

  void addOptionSelect(CLI::App *app, bool isRequired) {
    app->add_option("-s,--select", select)
       ->transform(CLI::CheckedTransformer(selectMap, CLI::ignore_case))
       ->description("Select best or worst based on the percentile argument.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addOptionPercentile(CLI::App *app, bool isRequired) {
    app->add_option("-p,--percentile", percentile)
       ->description("Percentile to clip above or below.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addOptionRoi(CLI::App *app, bool isRequired) {
    app->add_option("-r,--roi", roi)
       ->description("Portion of the image center to calculate sharpness.")
       ->required(isRequired)
       ->capture_default_str();
  }

  void addOptionDestination(CLI::App *app, bool isRequired) {
    app->add_option("-d,--destination", destination)
       ->description("Destination directory.")
       ->required(isRequired);
  }

  void addOptionFiles(CLI::App *app, bool isRequired) {
    app->add_option("-f,--files", files)
       ->description("Source files.")
       ->required(isRequired)
       ->check(CLI::ExistingFile);
  }
};
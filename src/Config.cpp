#include "Config.h"
#include "CLI11.hpp"

bool Config::parse(const int argc, const char **argv) {
  CLI::App app {};

  static const std::map<std::string, Select> selectMap = {
    {"better", Select::Better},
    {"worse", Select::Worse}
  };
  
  static const std::map<std::string, Operation> operationMap = {
    {"view", Operation::View},
    {"move", Operation::Move}
  };

  app.add_option("-s,--select", select)
    ->transform(CLI::CheckedTransformer(selectMap, CLI::ignore_case))
    ->description("Select best or worst based on the percentile argument.")
    ->required(true)
    ->capture_default_str();

  app.add_option("-p,--percentile", percentile)
    ->description("Percentile to clip above or below.")
    ->required(true)
    ->capture_default_str();
    
  app.add_option("-r,--roi", roi)
    ->description("Portion of the image center to calculate sharpness.")
    ->required(false)
    ->capture_default_str();

  app.add_option("-o,--operation", operation)
    ->transform(CLI::CheckedTransformer(operationMap, CLI::ignore_case))
    ->description("View statistics or move files.")
    ->required(true)
    ->capture_default_str();

  app.add_option("-d,--destination", destination)
    ->description("Destination directory.")
    ->required(true);

  app.add_option("-f,--files", files)
    ->description("Source files.")
    ->required(true)
    ->check(CLI::ExistingFile);

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    app.exit(e);
    return false;
  }

  return true;
}

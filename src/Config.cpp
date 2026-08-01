#include "Config.h"
#include "CLI11.hpp"

bool Config::parse(const int argc, const char **argv) {
  CLI::App app {};

  app.add_option("-f,--files", files)
    ->required(true)
    ->check(CLI::ExistingFile);

  app.add_option("-p,--percentile", percentile)
    ->required(false)
    ->capture_default_str();

  app.add_option("-t,--type", type)
    ->required(false)
    ->capture_default_str();

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    app.exit(e);
    return false;
  }

  return true;
}

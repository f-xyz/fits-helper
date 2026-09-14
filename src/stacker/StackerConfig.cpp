#include "StackerConfig.h"

void StackerConfig::bindSubcommands(
    CLI::App &app, const std::function<void(Subcommand)> &callback) {

  const auto stack = app.add_subcommand("stack");
  bindCommon(stack);
  bindStack(stack);
  stack->callback([&callback]() { callback(Subcommand::Stack); });

  const auto unstack = app.add_subcommand("unstack");
  bindCommon(unstack);
  unstack->callback([&callback]() { callback(Subcommand::Unstack); });
}

void StackerConfig::bindCommon(CLI::App *scmd) {
  scmd->add_option("-d,--dir", directory)
      ->description("Working directory.")
      ->required(true)
      ->transform(removeTerminalSlash)
      ->check(CLI::ExistingDirectory);
}

void StackerConfig::bindStack(CLI::App *scmd) {
  scmd->add_option("-n,--number", chunkSize)
      ->description("Number of images in a chunk.")
      ->required(false)
      ->capture_default_str();

  scmd->add_option("--script", sirilScript)
      ->description("Path to a Siril script.")
      ->required(false)
      ->capture_default_str()
      ->check(CLI::ExistingFile);

  scmd->add_option("--dark", dark)
      ->description("Dark frame.")
      ->required(false)
      ->check(CLI::ExistingFile);

  scmd->add_option("--flat", flat)
      ->description("Flat frame.")
      ->required(false)
      ->check(CLI::ExistingFile);

  scmd->add_option("--bias", bias)
      ->description("Bias frame.")
      ->required(false)
      ->check(CLI::ExistingFile);
}

std::string StackerConfig::removeTerminalSlash(const std::string &str) {
  return str.ends_with('/') ? str.substr(0, str.size() - 1) : str;
}
#include "Config.h"
#include "CLI11.hpp"
#include "Subcommand.h"

int Config::parse(const int argc, const char **argv,
                  const std::function<void(Subcommand)> &callback) {
  CLI::App app;
  app.require_subcommand(true);

  sorter.bindSubcommands(app, callback);
  stretcher.bindSubcommands(app, callback);
  stacker.bindSubcommands(app, callback);

  try {
    app.parse(argc, argv);
    return 0;
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }
}

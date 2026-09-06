#include "ScriptRunner.h"
#include "benchmarking/Timer.hpp"
#include "process.hpp"

using namespace utils;
using std::filesystem::path;

ScriptResult ScriptRunner::execute() {
  benchmarking::Timer<std::chrono::seconds> timer;

  return ScriptResult {
    .code = stack(),
    .integration = cleanup(),
    .seconds = timer.measure()
  };
}

int ScriptRunner::stack() {
  const auto command = getCommand();
  const auto result = process::exec(command);
  return result.code;
}

path ScriptRunner::cleanup() {
  const path tmp = "tmp";
  const path integration = "integration.fit";

  const path src = dir / tmp / integration;
  const path dst = dir / integration;

  std::filesystem::rename(src, dst);
  std::filesystem::remove_all(dir / "tmp");

  return dst;
}

std::string ScriptRunner::getCommand() {
  const std::string bash = "bash";
  const std::string stack = dir / "stack.sh";
  return bash + " " + stack;
}
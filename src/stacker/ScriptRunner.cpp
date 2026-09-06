#include "ScriptRunner.h"
#include "process.hpp"

using namespace utils;
using std::filesystem::path;

ScriptResult ScriptRunner::execute() {
  using namespace std::chrono;
  const auto start = steady_clock::now();

  result.code = stack();
  result.integration = cleanup();

  const auto end = steady_clock::now();
  result.seconds = duration_cast<seconds>(end - start);

  return result;
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
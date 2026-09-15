#include "ScriptRunner.h"
#include "string.hpp"
#include <benchmarking/Timer.hpp>
#include <filesystem>
#include <process.hpp>

using std::chrono::seconds;
using std::filesystem::path;
using utils::benchmarking::Timer;

ScriptResult ScriptRunner::execute(const std::filesystem::path &output) {
  Timer<seconds> timer;

  const auto code = stack();
  const auto seconds = timer.measure();

  if (code == 0) {
    copyIntegration(output);
    cleanup();
    return ScriptResult {.code = code,
                         .seconds = seconds};
  } else {
    cleanup();
    return ScriptResult {.code = code,
                         .seconds = seconds};
  }
}

int ScriptRunner::stack() {
  const auto command = getCommand();
  const auto result = utils::process::exec(command);
  return result.code;
}

void ScriptRunner::copyIntegration(const std::filesystem::path &output) {
  const path src = directory / "tmp" / "integration.fit";
  std::filesystem::rename(src, output);
}

void ScriptRunner::cleanup() {
  std::filesystem::remove_all(directory / "tmp");
}

std::string ScriptRunner::getCommand() {
  return "bash " + utils::string::quote(directory / "stack.sh");
}
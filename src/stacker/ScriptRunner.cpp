#include "ScriptRunner.h"
#include <astroutils/string.hpp>
#include <astroutils/benchmarking/Timer.hpp>
#include <filesystem>
#include <astroutils/process.hpp>

using std::chrono::seconds;
using std::filesystem::path;
using utils::benchmarking::Timer;
using utils::process::exec;
using utils::string::quote;
using utils::string::trim;

ScriptResult ScriptRunner::execute(const path &output,
                                   const Progress &progress) {
  Timer<seconds> timer;

  const auto code = stack(progress);
  const auto seconds = timer.measure();

  if (code == 0) {
    copyIntegration(output);
    cleanup();
    return ScriptResult {code, seconds};
  } else {
    cleanup();
    return ScriptResult {code, seconds};
  }
}

int ScriptRunner::stack(const Progress &progress) {
  const auto command = getCommand();
  const auto result = exec(command, [&progress](const std::string &message) {
    const auto command = "Running command: ";
    const auto index = message.find(command);
    if (index != std::string::npos) {
      progress(trim(message.substr(index)));
    }
  });

  return result.code;
}

void ScriptRunner::copyIntegration(const path &output) {
  const path src = directory / "tmp" / "integration.fit";
  std::filesystem::rename(src, output);
}

void ScriptRunner::cleanup() {
  std::filesystem::remove_all(directory / "tmp");
}

std::string ScriptRunner::getCommand() {
  return "bash " + quote(directory / "stack.sh");
}
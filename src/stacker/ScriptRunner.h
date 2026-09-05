#include "benchmark.hpp"
#include "process.hpp"
#include <stdexcept>

using namespace utils;

class ScriptRunner {
  std::filesystem::path dir;
  int index = 0;

public:
  ScriptRunner(const std::filesystem::path &dir, int index)
      : dir(dir), index(index) {}

  void execute() {
    const auto process = std::bind(&ScriptRunner::stack, this);
    const auto seconds = benchmark<std::chrono::seconds>(process);
    std::println("  Time: {} sec", seconds);
  }

  void stack() {
    const std::string bash = "/usr/bin/bash";
    const std::string stack = (dir / "stack.sh").string();
    const std::string command = bash + " " + stack;
    std::println("  Command: {}", command);

    const auto result = process::exec(command);
    std::println("  Code: {}", result.code);

    if (result.code == 0) {
      // const auto name = std::to_string(index) + ".fit";
      // std::filesystem::rename(dir / "tmp/integration.fit",
      //                         dir / "../masters" / name);
      std::filesystem::rename(dir / "tmp/integration.fit",
                              dir / "integration.fit");
      std::filesystem::remove_all(dir / "tmp");
    } else {
      throw std::runtime_error("Siril has failed, see siril.log");
    }
  }

private:
};
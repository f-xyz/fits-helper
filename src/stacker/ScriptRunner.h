#pragma once

#include <chrono>
#include <filesystem>
#include <string>

using std::filesystem::path;
using Progress = std::function<void(const std::string &)>;

struct ScriptResult {
  int code;
  std::chrono::seconds seconds;
};

class ScriptRunner {
  std::filesystem::path directory;

public:
  explicit ScriptRunner(const path &dir) : directory(dir) {}
  ScriptResult execute(const path &output, const Progress &progress);

private:
  int stack(const Progress &progress);
  void copyIntegration(const path &output);
  void cleanup();
  std::string getCommand();
};
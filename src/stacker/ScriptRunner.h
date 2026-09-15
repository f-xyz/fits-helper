#pragma once

#include <chrono>
#include <filesystem>
#include <string>

struct ScriptResult {
  int code;
  std::chrono::seconds seconds;
};

class ScriptRunner {
  std::filesystem::path directory;

public:
  explicit ScriptRunner(const std::filesystem::path &dir) : directory(dir) {}
  ScriptResult execute(const std::filesystem::path &output);

private:
  int stack();
  void copyIntegration(const std::filesystem::path &output);
  void cleanup();
  std::string getCommand();
};
#pragma once

struct ScriptResult {
  int code;
  std::filesystem::path integration;
  std::chrono::seconds seconds;
};

class ScriptRunner {
  std::filesystem::path dir;
  ScriptResult result;

public:
  explicit ScriptRunner(const std::filesystem::path &dir) : dir(dir) {}

  ScriptResult execute();

private:
  int stack();
  std::filesystem::path cleanup();
  std::string getCommand();
};
#pragma once

#include "../Config.h"

class ScriptGenerator {
  // Internal scripts
  std::filesystem::path sirilScript;
  std::filesystem::path shellScript;
  // Calibration frames
  std::filesystem::path bias;
  std::filesystem::path dark;
  std::filesystem::path flat;

public:
  explicit ScriptGenerator(const Config::StackerConfig &config)
      : sirilScript(config.sirilScript), shellScript(config.shellScript),
        bias(config.bias), dark(config.dark), flat(config.flat) {}

  std::string getStackerScript(const std::filesystem::path &chunkDir);
  std::string getShellScript(const std::filesystem::path &chunkDir);

private:
  std::string getCalibration();
};
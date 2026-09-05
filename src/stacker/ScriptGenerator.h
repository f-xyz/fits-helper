#include "../Config.h"
#include "fs.hpp"
#include "string.hpp"

using namespace utils;

class ScriptGenerator {
  // Internal scripts
  std::filesystem::path sirilScript;
  std::filesystem::path stackScript;
  // Calibration frames
  std::filesystem::path bias;
  std::filesystem::path dark;
  std::filesystem::path flat;

public:
  explicit ScriptGenerator(const Config::StackerConfig &config)
      : sirilScript(config.sirilScript), stackScript(config.stackScript),
        bias(config.bias), dark(config.dark), flat(config.flat) {}

  std::string getStackerScript(const std::filesystem::path &chunkDir) {
    const std::map<std::string, std::string> vars = {
      {"${PATH}", std::filesystem::canonical(chunkDir)},
      {"${CALIBRATION}", getCalibration()}
    };

    std::string script = fs::readText(sirilScript);
    for (const auto &[key, value] : vars) {
      script = string::replace_all(script, key, value);
    }

    return script;
  }

  std::string getShellScript(const std::filesystem::path &chunkDir) {
    const std::map<std::string, std::string> vars = {
      {"${PATH}", std::filesystem::canonical(chunkDir)}
    };

    std::string script = fs::readText(stackScript);
    for (const auto &[key, value] : vars) {
      script = string::replace_all(script, key, value);
    }

    return script;
  }

private:
  std::string getCalibration() {
    const std::map<std::string, std::string> vars = {
      {"-bias", bias},
      {"-dark", dark},
      {"-flat", flat}
    };

    std::vector<std::string> result;
    for (const auto &[key, value] : vars) {
      if (!value.empty()) {
        std::string path = std::filesystem::canonical(value);
        std::string item = key + "=" + path;
        result.push_back(item);
      }
    }

    return string::join(result, " ");
  }
};
#include "ScriptGenerator.h"
#include "fs.hpp"
#include "string.hpp"

using namespace utils;
using std::filesystem::path;

std::string ScriptGenerator::getStackerScript(const path &chunkDir) {
  const std::map<std::string, std::string> vars = {
      {"${PATH}", std::filesystem::canonical(chunkDir)},
      {"${CALIBRATION}", getCalibration()}};

  std::string script = fs::readText(sirilScript);
  for (const auto &[key, value] : vars) {
    script = string::replace_all(script, key, value);
  }

  return script;
}

std::string ScriptGenerator::getShellScript(const path &chunkDir) {
  const std::map<std::string, std::string> vars = {
      {"${PATH}", std::filesystem::canonical(chunkDir)}};

  std::string script = fs::readText(shellScript);
  for (const auto &[key, value] : vars) {
    script = string::replace_all(script, key, value);
  }

  return script;
}

std::string ScriptGenerator::getCalibration() {
  const std::map<std::string, std::string> vars = {
      {"-bias", bias}, {"-dark", dark}, {"-flat", flat}};

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
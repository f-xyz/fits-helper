#include "ScriptGenerator.h"
#include <astroutils/fs.hpp>
#include <map>
#include <astroutils/string.hpp>
#include <vector>

using std::filesystem::path;

std::string ScriptGenerator::getStackerScript(const path &chunkDir) {
  const std::map<std::string, std::string> vars = {
    {"${PATH}", std::filesystem::canonical(chunkDir)},
    {"${CALIBRATION}", getCalibration()}
  };

  std::string script = astroutils::fs::readText(sirilScript);
  for (const auto &[key, value] : vars) {
    script = astroutils::string::replace_all(script, key, value);
  }

  return script;
}

std::string ScriptGenerator::getShellScript(const path &chunkDir) {
  const std::map<std::string, std::string> vars = {
    {"${PATH}", std::filesystem::canonical(chunkDir)}
  };

  std::string script = astroutils::fs::readText(shellScript);
  for (const auto &[key, value] : vars) {
    script = astroutils::string::replace_all(script, key, value);
  }

  return script;
}

std::string ScriptGenerator::getCalibration() {
  const std::map<std::string, std::string> vars = {
    {"-bias", bias},
    {"-dark", dark},
    {"-flat", flat}
  };

  std::vector<std::string> result;
  for (const auto &[key, value] : vars) {
    if (!value.empty()) {
      std::string path = std::filesystem::canonical(value);
      std::string item = key;
      item += "=" + path;
      result.push_back(item);
    }
  }

  return astroutils::string::join(result, " ");
}
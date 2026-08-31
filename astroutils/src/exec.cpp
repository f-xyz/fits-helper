#include "exec.hpp"
#include <array>

namespace utils::process {

ExecResult exec(const std::string &command) {
  std::string output;
  std::array<char, 128> buffer;

  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return {"", -1};
  }

  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    output += buffer.data();
  }

  int rawCode = pclose(pipe);
  int exitCode = WIFEXITED(rawCode) ? WEXITSTATUS(rawCode) : -1;

  return {output, exitCode};
}

} // namespace utils::process
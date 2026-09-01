#include "process.hpp"
#include <array>
#include <string>

namespace utils::process {

ExecResult exec(const std::string &command, ExecCallback callback) {
  std::string output;
  std::array<char, 128> buffer;

  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return {"", -1};
  }

  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    const char *data = buffer.data();
    output += buffer.data();
    if (callback.has_value()) {
      (*callback)(data);
    }
  }

  int rawCode = pclose(pipe);
  int exitCode = WIFEXITED(rawCode) ? WEXITSTATUS(rawCode) : -1;

  return {output, exitCode};
}

} // namespace utils::process
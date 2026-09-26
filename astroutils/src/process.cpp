#include <astroutils/process.hpp>
#include <array>
#include <cstdio>
#include <sys/wait.h>

namespace astroutils::process {

ExecResult exec(std::string_view command, ExecCallback callback) {
  std::string output;
  const std::string ownedCommand(command);
  std::array<char, 128> buffer;

  FILE *pipe = popen(ownedCommand.c_str(), "r");
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

} // namespace astroutils::process

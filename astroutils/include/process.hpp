#pragma once

namespace utils::process {

struct ExecResult {
  std::string output;
  int code = -1;
};

using ExecCallback = std::optional<std::function<void(const char *)>>;

ExecResult exec(const std::string &command,
                ExecCallback callback = std::nullopt);

}; // namespace utils::process
#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace astroutils::process {

struct ExecResult {
  std::string output;
  int code = -1;
};

using ExecCallback = std::optional<std::function<void(const char *)>>;

ExecResult exec(std::string_view command,
                ExecCallback callback = std::nullopt);

}; // namespace astroutils::process

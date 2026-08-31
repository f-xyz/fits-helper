#pragma once

#include <string>

namespace utils::process {

struct ExecResult {
  std::string output;
  int code = -1;
};

ExecResult exec(const std::string &command);

}; // namespace utils::process
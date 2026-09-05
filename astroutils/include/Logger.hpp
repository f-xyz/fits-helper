#pragma once

#include "cli/colors.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include <string>
#include <utility>

namespace utils::logging {

class Logger {
  std::ofstream file;
  std::mutex mutex;

public:
  explicit Logger(const std::filesystem::path &logFilePath) {
    file.open(logFilePath, std::ios_base::trunc);
  }

  template <typename... Args>
  void header(const std::format_string<Args...> &fmt, Args &&...args) {
    auto message = std::format(fmt, std::forward<Args>(args)...);
    printLine(utils::cli::bold(message));
    writeLine(message, "INFO");
  }

  template <typename... Args>
  void info(const std::format_string<Args...> &fmt, Args &&...args) {
    auto message = std::format(fmt, std::forward<Args>(args)...);
    printLine(message);
    writeLine(message, "INFO");
  }

  template <typename... Args>
  void debug(const std::format_string<Args...> &fmt, Args &&...args) {
    auto message = std::format(fmt, std::forward<Args>(args)...);
    writeLine(message, "DEBUG");
  }

  template <typename... Args>
  void error(const std::format_string<Args...> &fmt, Args &&...args) {
    auto message = std::format(fmt, std::forward<Args>(args)...);
    printLine(message, 128, 0, 0);
    writeLine(message, "ERROR");
  }

  template <typename... Args>
  void success(const std::format_string<Args...> &fmt, Args &&...args) {
    auto message = std::format(fmt, std::forward<Args>(args)...);
    printLine(message, 0, 128, 0);
    writeLine(message, "INFO");
  }

protected:
  static void printLine(const std::string &line);

  static void printLine(const std::string &line, uint r, uint g, uint b);

  void writeLine(const std::string &message,
                 const std::string &severity = "INFO");
};

} // namespace utils::logging
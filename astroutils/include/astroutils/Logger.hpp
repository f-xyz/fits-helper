#pragma once

#include "cli/colors.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>

namespace astroutils::logging {

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
    printLine(astroutils::cli::bold(message));
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
    printLine(message, 192, 0, 0);
    writeLine(message, "ERROR");
  }

  template <typename... Args>
  void success(const std::format_string<Args...> &fmt, Args &&...args) {
    auto message = std::format(fmt, std::forward<Args>(args)...);
    printLine(message, 0, 192, 0);
    writeLine(message, "INFO");
  }

protected:
  static void printLine(std::string_view line);
  static void printLine(std::string_view line, unsigned int r, unsigned int g, unsigned int b);

  void writeLine(std::string_view message,
                 std::string_view severity = "INFO");
};

} // namespace astroutils::logging

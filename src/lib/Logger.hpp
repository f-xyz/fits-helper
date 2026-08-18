#pragma once

#include "cli/colors.hpp"
#include "string.hpp"

class Logger {
  std::ofstream file;

public:
  explicit Logger(const std::filesystem::path &logFilePath) {
    file.open(logFilePath, std::ios_base::trunc);
  }

  template <typename... Args>
  void header(const std::format_string<Args...> &fmt, Args &&...args) {
    auto message = std::format(fmt, std::forward<Args>(args)...);
    printLine(bold(message));
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
  static void printLine(const std::string &line) {
    std::println("{}", line); 
  }

  static void printLine(const std::string &line, uint r, uint g, uint b) {
    std::println("{}", rgb(line, r, g, b));
  }

  void writeLine(const std::string &message, const std::string &severity = "INFO") {
    auto now = std::chrono::system_clock::now();
    auto time = std::format("{:%Y-%m-%d %H:%M:%S}", now);
    auto clean = utils::string::trimAndRemoveColors(message);
    auto line = std::format("[{}] {} {}", time, severity, clean);

    file.write((line + "\n").c_str(), line.size() + 1);
  }
};

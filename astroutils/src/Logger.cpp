#include "astroutils/cli/colors.hpp"
#include <astroutils/Logger.hpp>
#include <astroutils/string.hpp>
#include <chrono>
#include <format>
#include <mutex>
#include <print>
#include <string_view>

namespace astroutils::logging {

void Logger::printLine(std::string_view line) { std::println("{}", line); }

void Logger::printLine(std::string_view line, byte r, byte g, byte b) {
  std::println("{}", astroutils::cli::rgb(line, r, g, b));
}

void Logger::writeLine(std::string_view message, std::string_view severity) {
  auto now = std::chrono::system_clock::now();
  auto time = std::format("{:%Y-%m-%d %H:%M:%S}", now);
  auto clean = astroutils::string::trim(astroutils::cli::removeColors(message));
  auto line = std::format("[{}] {} {}", time, severity, clean);

  std::lock_guard<std::mutex> lock(mutex);
  file << line << "\n";
  file.flush();
}

} // namespace astroutils::logging

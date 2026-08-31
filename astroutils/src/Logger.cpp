#include "Logger.hpp"
#include "string.hpp"
#include <chrono>
#include <print>

namespace utils::logging {

void Logger::printLine(const std::string &line) { std::println("{}", line); }

void Logger::printLine(const std::string &line, uint r, uint g, uint b) {
  std::println("{}", utils::cli::rgb(line, r, g, b));
}

void Logger::writeLine(const std::string &message,
                       const std::string &severity) {
  auto now = std::chrono::system_clock::now();
  auto time = std::format("{:%Y-%m-%d %H:%M:%S}", now);
  auto clean = utils::string::trim(utils::cli::removeColors(message));
  auto line = std::format("[{}] {} {}", time, severity, clean);

  std::lock_guard<std::mutex> lock(mutex);
  file << line << "\n";
  file.flush();
}

} // namespace utils::logging
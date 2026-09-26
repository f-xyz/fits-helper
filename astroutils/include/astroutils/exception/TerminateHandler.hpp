#pragma once

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <format>
#include <mutex>
#include <print>
#include <stacktrace>
#include <string>

namespace astroutils::exception {

class TerminateHandler final {
  inline static std::atomic_flag isHandled = false;

public:
  static void install() {
    static std::once_flag once;
    std::call_once(once, []() {
      std::set_terminate(onTerminate);
      std::signal(SIGSEGV, onSignal);
      std::signal(SIGILL, onSignal);
      std::signal(SIGFPE, onSignal);
      std::signal(SIGBUS, onSignal);
    });
  }

private:
  TerminateHandler() = default;

  static void onTerminate() {
    if (!isHandled.test_and_set()) {
      std::println(stderr, "{}", getMessage());
      std::println(stderr, "{}", std::stacktrace::current(4));
      std::abort();
    }
  }

  static void onSignal(int signal) {
    if (!isHandled.test_and_set()) {
      std::println(stderr, "Signal received: {} {}", signal, strsignal(signal));
      std::println(stderr, "{}", std::stacktrace::current(2));
    }
    std::signal(signal, SIG_DFL);
    std::raise(signal);
  }

  static std::string getMessage() {
    const auto exception = std::current_exception();
    if (exception) {
      try {
        std::rethrow_exception(exception);
      } catch (const std::exception &e) {
        return std::format("Exception: {}", e.what());
      } catch (...) {
        return std::format("Unknown exception.");
      }
    } else {
      return std::format("std::terminate() has been called.");
    }
  }
};

} // namespace astroutils::exception
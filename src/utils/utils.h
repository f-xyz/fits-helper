#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <future>
#include <string>
#include <thread>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include "colors.h"

namespace utils {

  class str {
  public:
    static constexpr std::string uppercase(const std::string &str) {
      std::locale locale("C.UTF-8");
      std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      std::wstring ws = converter.from_bytes(str);

      const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
      facet.toupper(&ws[0], &ws[0] + ws.size());

      return converter.to_bytes(ws);
    }

    static constexpr std::string lowercase(const std::string &str) {
      std::locale locale("C.UTF-8");
      std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      std::wstring ws = converter.from_bytes(str);

      const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
      facet.tolower(&ws[0], &ws[0] + ws.size());

      return converter.to_bytes(ws);
    }

    static constexpr std::string ltrim(const std::string &str) {
      const auto pos = str.find_first_not_of(" \t\r\n");
      return str.substr(std::min(pos, str.size()));
    }

    static constexpr std::string rtrim(const std::string &str) {
      const auto pos = str.find_last_not_of(" \t\r\n");
      return str.substr(0, pos + 1);
    }

    static constexpr std::string trim(const std::string &str) {
      return ltrim(rtrim(str));
    }

    static constexpr std::string trimAndRemoveColors(const std::string &str) {
      return trim(removeColors(str));
    }

    static constexpr std::string formatNumber(double x) {
      const auto str = std::to_string(x);
      if (x > 0) {
        return std::format("{}", rgb(str, 0, 128, 0));
      } else if (x < 0) {
        return std::format("{}", rgb(str, 128, 0, 0));
      } else {
        return std::format("{}", str);
      }
    }
  };

  class fs {
  public:
    static constexpr std::vector<std::string> readDir(const std::filesystem::path &dir) {
      if (!std::filesystem::exists(dir)) {
        return {};
      }

      std::vector<std::string> result;
      std::filesystem::directory_iterator iterator(dir);
      for (const auto &file : iterator) {
        if (file.is_regular_file()) {
          result.push_back(file.path());
        }
      }

      std::ranges::sort(result);
      return result;
    }
  };

  class async {
  public:
    static std::future<void> setTimeout(const std::function<void()>& callback, std::chrono::milliseconds delay) {
      std::promise<void> promise;
      std::future<void> future = promise.get_future();

      std::thread([delay, callback, promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(delay);
        callback();
        promise.set_value();
      }).detach();

      return future;
    }
  };

};
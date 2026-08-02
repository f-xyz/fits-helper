#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include "colors.h"

namespace utils {

  class str {
  public:
    static constexpr std::string_view ltrim(std::string_view str) {
      const auto pos = str.find_first_not_of(" \t\r\n");
      return str.substr(std::min(pos, str.size()));
    }

    static constexpr std::string_view rtrim(std::string_view str) {
      const auto pos = str.find_last_not_of(" \t\r\n");
      return str.substr(0, pos + 1);
    }

    static constexpr std::string_view trim(std::string_view str) {
      return ltrim(rtrim(str));
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
    static void setTimeout(const std::function<void()>& callback, std::chrono::milliseconds delay) {
      std::thread([delay, callback]() {
        std::this_thread::sleep_for(delay);
        callback();
      }).detach();
    }
  };

};
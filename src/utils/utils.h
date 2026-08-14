#pragma once

#include "../fits/FitsReader.h"
#include "colors.h"
#include <chrono>
#include <filesystem>
#include <functional>
#include <future>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

namespace utils {
  namespace string {
    inline std::string uppercase(const std::string &str) {
      if (str.empty()) {
        return "";
      }

      std::locale locale("C.UTF-8");
      std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      std::wstring ws = converter.from_bytes(str);

      const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
      facet.toupper(&ws[0], &ws[0] + ws.size());

      return converter.to_bytes(ws);
    }

    inline std::string lowercase(const std::string &str) {
      if (str.empty()) {
        return "";
      }

      std::locale locale("C.UTF-8");
      std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      std::wstring ws = converter.from_bytes(str);

      const auto &facet = std::use_facet<std::ctype<wchar_t>>(locale);
      facet.tolower(&ws[0], &ws[0] + ws.size());

      return converter.to_bytes(ws);
    }

    constexpr std::string ltrim(const std::string &str) {
      const auto pos = str.find_first_not_of(" \t\r\n");
      return str.substr(std::min(pos, str.size()));
    }

    constexpr std::string rtrim(const std::string &str) {
      const auto pos = str.find_last_not_of(" \t\r\n");
      return str.substr(0, pos + 1);
    }

    constexpr std::string trim(const std::string &str) {
      return ltrim(rtrim(str));
    }

    constexpr std::string trimAndRemoveColors(const std::string &str) {
      return trim(removeColors(str));
    }

    constexpr std::string formatNumber(double x) {
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

  namespace fs {
    inline std::vector<std::string> readDir(const std::filesystem::path &dir) {
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

    inline std::string getTmpFileName(const std::string &dir) {
      std::vector<std::string> existing = readDir(dir);
      std::vector<int> indexes;

      std::ranges::transform(existing,
        std::back_inserter(indexes),
        [](const std::string &file) {
          std::smatch matches;
          std::string name = std::filesystem::path(file).filename().string();
          std::regex regex = std::regex(R"((\d+).*?$)");
          std::regex_search(name, matches, regex);
          int index = matches.size() ? std::stoi(matches[0].str()) : 0;
          return index;
      });

      int maxIndex = indexes.empty() ? 0 : *std::ranges::max_element(indexes);
      int newIndex = maxIndex + 1;

      std::string out = dir.ends_with('/') ? dir : dir + '/';
      return std::format("{}{}", out, newIndex);
    }
  }

  namespace images {
    inline cv::Mat readImage(const std::string &file) {
      std::string ext = std::filesystem::path(file).extension().string();
      return ext == ".fit" || ext == ".fits"
        ? FitsReader().read(file)
        : cv::imread(file);
    }

    inline std::pair<double, double> getImageMinMax(const cv::Mat &image) {
      double min, max;
      cv::minMaxLoc(image, &min, &max);
      return {min, max};
    }

    inline std::string getImageInfo(const cv::Mat &image) {
      auto type = cv::typeToString(image.type());
      auto minmax = getImageMinMax(image);

      return std::format("{} {}x{} [{}-{}]",
        type, image.cols, image.rows, minmax.first, minmax.second);
    }

    inline std::vector<float> getImageHistogram(const cv::Mat &image, int histSize = 16) {
      cv::Mat lab;
      cv::cvtColor(image, lab, cv::COLOR_BGR2Lab);

      const int channels[] = {0};
      const auto minmax = getImageMinMax(image);
      const float min = static_cast<float>(minmax.first);
      const float max = static_cast<float>(minmax.second + 1e-6);
      const float range[] = {min, max};
      const float *ranges[] = {range};

      cv::Mat hist;
      cv::calcHist(&lab, 1, channels, cv::noArray(),
        hist, 1, &histSize, ranges);

      // Converts cv::Mat<float> -> std::vector<float>
      return hist;
    }
  };

  namespace async {
    template <typename T>
    inline std::future<T> setTimeout(const std::function<T()>& callback, std::chrono::milliseconds delay) {
      std::promise<T> promise;
      std::future<T> future = promise.get_future();

      std::thread([delay, callback, promise = std::move(promise)]() mutable {
        std::this_thread::sleep_for(delay);
        if constexpr (std::is_void<T>()) {
          callback();
          promise.set_value();
        } else {
          promise.set_value(callback());
        }
      }).detach();

      return future;
    }
  };
};
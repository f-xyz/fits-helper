#pragma once

#include <chrono>
#include <future>
#include <thread>

namespace utils::async {
  template<typename T>
  std::future<T> setTimeout(const std::function<T()> &callback,
                            std::chrono::milliseconds delay) {
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

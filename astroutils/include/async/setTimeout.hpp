#pragma once

#include <chrono>
#include <future>
#include <thread>
#include <type_traits>
#include <utility>

namespace utils::async {

template <typename F>
auto setTimeout(F&& callback, std::chrono::milliseconds delay) {
  using T = std::invoke_result_t<F>;
  std::promise<T> promise;
  std::future<T> future = promise.get_future();

  std::thread([
    delay = std::chrono::milliseconds(delay),
    callback = std::forward<F>(callback),
    promise = std::move(promise)
  ]() mutable {
    std::this_thread::sleep_for(delay);
    if constexpr (std::is_void_v<T>) {
      callback();
      promise.set_value();
    } else {
      promise.set_value(callback());
    }
  }).detach();

  return future;
}

}; // namespace utils::async

#include "async/Throttle.hpp"
#include "async/setTimeout.hpp"

using namespace std::chrono_literals;

TEST(AsyncThrottle, CallInvokesCallbackAtMostOncePerWindow) {
  // arrange
  std::atomic<int> calls{0};
  utils::async::Throttle throttle(20ms, [&] { ++calls; });

  // act
  throttle.call();
  throttle.call();
  throttle.call();
  std::this_thread::sleep_for(60ms);

  // assert
  EXPECT_EQ(calls.load(), 1);
}

TEST(AsyncThrottle, SetTimeoutResolvesAfterDelay) {
  // arrange
  std::atomic<int> calls {0};
  auto future = utils::async::setTimeout(
      [&] {
        ++calls;
        return 7;
      },
      20ms);

  // act
  const auto status = future.wait_for(0ms);
  const auto result = future.get();

  // assert
  EXPECT_EQ(status, std::future_status::timeout);
  EXPECT_EQ(result, 7);
  EXPECT_EQ(calls.load(), 1);
}

#include "benchmarking/Timer.hpp"

TEST(BenchmarkingTimer, measure) {
  // arrange
  utils::benchmarking::Timer timer;
  // act
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  auto result = timer.measure();
  // assert
  EXPECT_EQ(result, std::chrono::milliseconds(1));
};
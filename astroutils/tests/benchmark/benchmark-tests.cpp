#include "benchmark.hpp"

TEST(Benchmark, benchmark) {
  // arrange
  // act
  auto result = utils::benchmark([]() { 
    std::this_thread::sleep_for(std::chrono::seconds(1));
  });
  // assert
  EXPECT_EQ(result, std::chrono::seconds(1));
};
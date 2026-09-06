#include "benchmarking/benchmark.hpp"

TEST(BenchmarkingBenchmark, benchmark) {
  // arrange
  // act
  auto result = utils::benchmarking::benchmark([]() { 
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  });
  // assert
  EXPECT_EQ(result, std::chrono::milliseconds(1));
};
#include "cli/spark.hpp"
#include "gmock/gmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(CliSpark, EmptyRangeReturnsPlaceholder) {
  // arrange
  const std::vector<int> empty = {};
  // act
  const auto result = utils::cli::spark(empty);
  // assert
  EXPECT_EQ(result, "[empty]");
}

TEST(CliSpark, ReturnsColoredBars) {
  // arrange
  const std::vector<int> numbers = {0, 1, 2};
  // act
  const auto result = utils::cli::spark(numbers);
  // assert
  EXPECT_THAT(result, testing::StartsWith("["));
  EXPECT_THAT(result, testing::HasSubstr("\033[38;2;255;0;0m▁\033[0m"));
  EXPECT_THAT(result, testing::HasSubstr("\033[38;2;255;127;32m▄\033[0m"));
  EXPECT_THAT(result, testing::HasSubstr("\033[38;2;255;255;64m█\033[0m"));
  EXPECT_THAT(result, testing::EndsWith("]"));
}

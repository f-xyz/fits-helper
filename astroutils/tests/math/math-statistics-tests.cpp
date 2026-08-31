#include "../../include/math.hpp"
#include "gtest/gtest.h"
#include <cmath>

TEST(MathDescriptiveStatistics, sum) {
  // arrange
  std::vector<int> data = {1, 2, 3, 4};
  // act
  auto result = utils::math::sum(data);
  // assert
  EXPECT_EQ(result, 10);
};

TEST(MathDescriptiveStatistics, mean) {
  // arrange
  std::vector<int> data = {1, 2, 3, 4};
  // act
  auto result = utils::math::mean(data);
  // assert
  EXPECT_EQ(result, 2.5);
};

TEST(MathDescriptiveStatistics, median_even) {
  // arrange
  std::vector<int> data = {1, 2, 3, 4};
  // act
  auto result = utils::math::median(data);
  // assert
  EXPECT_EQ(result, 2.5);
};

TEST(MathDescriptiveStatistics, median_odd) {
  // arrange
  std::vector<int> data = {1, 2, 3};
  // act
  auto result = utils::math::median(data);
  // assert
  EXPECT_EQ(result, 2);
};

TEST(MathDescriptiveStatistics, variance) {
  // arrange
  std::vector<int> data = {1, 2, 3, 4};
  // act
  auto result = utils::math::variance(data);
  // assert
  EXPECT_EQ(result, 1.25);
};

TEST(MathDescriptiveStatistics, variance_empty_range) {
  // arrange
  std::vector<int> data = {};
  // act
  auto result = utils::math::variance(data);
  // assert
  EXPECT_EQ(result, 0);
};

TEST(MathDescriptiveStatistics, stddev) {
  // arrange
  std::vector<int> data = {1, 2, 3, 4};
  // act
  auto result = utils::math::stddev(data);
  // assert
  EXPECT_EQ(result, std::sqrt(1.25));
};
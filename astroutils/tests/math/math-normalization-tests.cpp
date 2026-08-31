#include <gtest/gtest.h>

#include "../../include/math.hpp"

TEST(MathNorm, normalize_with_2_arguments) {
  // arrange
  double value = 3.14;
  // act
  auto result = utils::math::normalize(value, 0, 10);
  // assert
  EXPECT_EQ(result, 0.314);
};

TEST(MathNorm, normalize_with_2_equal_arguments) {
  // arrange
  double value = 3.14;
  // act
  auto result = utils::math::normalize(value, 10, 10);
  // assert
  EXPECT_EQ(result, 0.5);
};

TEST(MathNorm, normalize_with_minmax_result) {
  // arrange
  double value = 3.14;
  auto minmax = std::ranges::minmax(std::vector<int>({0, 10}));
  // act
  auto result = utils::math::normalize(value, minmax);
  // assert
  EXPECT_EQ(result, 0.314);
};

TEST(MathNorm, normalize_with_std_pair) {
  // arrange
  double value = 3.14;
  std::pair<int, int> pair(0, 10);
  // act
  auto result = utils::math::normalize(value, pair);
  // assert
  EXPECT_EQ(result, 0.314);
};

TEST(MathNorm, normalize_with_range) {
  // arrange
  std::vector<int> range = {1, 2, 3, 4, 5};
  // act
  auto result = utils::math::normalize(range);
  // assert
  EXPECT_EQ(result, std::vector<double>({0, 0.25, 0.5, 0.75, 1}));
};

TEST(MathNorm, rescale) {
  // arrange
  std::vector<int> range = {0, 5, 5, 10};
  // act
  auto result = utils::math::rescale(range);
  // assert
  EXPECT_EQ(result, std::vector<double>({0, 0.25, 0.25, 0.5}));
};

TEST(MathNorm, rescale_with_zero_sum) {
  // arrange
  std::vector<int> range = {0, 5, -5, 0};
  // act
  auto result = utils::math::rescale(range);
  // assert
  EXPECT_EQ(result, std::vector<double>({0.25, 0.25, 0.25, 0.25}));
};
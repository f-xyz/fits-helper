#include "math.hpp"

TEST(MathRandom, random_double) {
  // arrange
  const double min = 5;
  const double max = 10;
  // act
  const auto result = utils::math::random(min, max);
  // assert
  EXPECT_GE(result, min);
  EXPECT_LT(result, max);
};

TEST(MathRandom, random_size_t) {
  // arrange
  const std::size_t min = 5;
  const std::size_t max = 10;
  // act
  const auto result = utils::math::random(min, max);
  // assert
  EXPECT_GE(result, min);
  EXPECT_LT(result, max);
};
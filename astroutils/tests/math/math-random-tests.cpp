#include "math.hpp"

TEST(MathRandom, random) {
  // arrange
  const double min = 5;
  const double max = 10;
  // act
  const auto result = utils::math::random(min, max);
  // assert
  EXPECT_GE(result, min);
  EXPECT_LT(result, max);
};
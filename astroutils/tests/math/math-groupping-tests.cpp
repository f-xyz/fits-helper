#include "math.hpp"

TEST(MathGroupping, countBy) {
  // arrange
  std::vector<int> range = {1, 2, 3, 4, 5};
  // act
  const auto isEven = [](auto x) { return x % 2 == 0; };
  const auto result = utils::math::countBy(range, isEven);
  // assert
  const std::map<bool, int> expected({{false, 3}, {true, 2}});
  EXPECT_EQ(result, expected);
};

TEST(MathGroupping, groupBy) {
  // arrange
  const std::vector<int> range = {1, 2, 3, 4, 5};
  // act
  const auto isEven = [](auto x) { return x % 2 == 0; };
  const auto result = utils::math::groupBy(range, isEven);
  // assert
  const std::map<bool, std::vector<int>> expected(
      {{false, {1, 3, 5}}, {true, {2, 4}}});
  EXPECT_EQ(result, expected);
};
#include "math.hpp"

TEST(MathSlicing, slice_begin) {
  // arrange
  std::vector<int> data = {1, 2, 3, 4};
  // act
  auto result = utils::math::slice(data, 2);
  // assert
  std::vector<int> expected = {3, 4};
  EXPECT_EQ(result, expected);
};

TEST(MathSlicing, slice_begin_negative) {
  // arrange
  std::vector<int> data = {1, 2, 3, 4};
  // act
  auto result = utils::math::slice(data, -2);
  // assert
  std::vector<int> expected = {3, 4};
  EXPECT_EQ(result, expected);
};

TEST(MathSlicing, slice_begin_end) {
  // arrange
  std::vector<int> data = {1, 2, 3, 4};
  // act
  auto result = utils::math::slice(data, 1, 3);
  // assert
  EXPECT_EQ(result, std::vector<int>({2, 3}));
};

TEST(MathSlicing, slice_begin_end_negative) {
  // arrange
  std::vector<int> data = {1, 2, 3, 4};
  // act
  auto result = utils::math::slice(data, -3, -1);
  // assert
  EXPECT_EQ(result, std::vector<int>({2, 3}));
};
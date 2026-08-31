#include "string.hpp"
#include <gtest/gtest.h>

TEST(StringSlice, slice_startOnly_positive) {
  // arrange
  std::string str = "hello world";
  // act
  auto result = utils::string::slice(str, 6);
  // assert
  EXPECT_EQ(result, "world");
}

TEST(StringSlice, slice_startOnly_negative) {
  // arrange
  std::string str = "hello world";
  // act
  auto result = utils::string::slice(str, -5);
  // assert
  EXPECT_EQ(result, "world");
}

TEST(StringSlice, slice_startAndEnd_positive) {
  // arrange
  std::string str = "hello world";
  // act
  auto result = utils::string::slice(str, 0, 5);
  // assert
  EXPECT_EQ(result, "hello");
}

TEST(StringSlice, slice_startAndEnd_negativeEnd) {
  // arrange
  std::string str = "hello world";
  // act
  auto result = utils::string::slice(str, 6, -1);
  // assert
  EXPECT_EQ(result, "worl");
}

TEST(StringSlice, slice_startAndEnd_bothNegative) {
  // arrange
  std::string str = "hello world";
  // act
  auto result = utils::string::slice(str, -5, -1);
  // assert
  EXPECT_EQ(result, "worl");
}

TEST(StringSlice, slice_startAndEnd_equalIndices) {
  // arrange
  std::string str = "hello world";
  // act
  auto result = utils::string::slice(str, 4, 4);
  // assert
  EXPECT_EQ(result, "");
}
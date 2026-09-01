#include "string.hpp"
#include <gtest/gtest.h>

TEST(StringRegex, regex_fullMatch) {
  // arrange
  std::regex rx(R"(\d+)");
  std::string input = "qwe123asd";
  // act
  auto result = utils::string::match(rx, input);
  // assert
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "123");
}

TEST(StringRegex, regex_captureGroups) {
  // arrange
  std::regex rx(R"((\w+):(\d+))");
  std::string input = "user:42";
  // act
  auto fullMatch = utils::string::match(rx, input, 0);
  auto group1 = utils::string::match(rx, input, 1);
  auto group2 = utils::string::match(rx, input, 2);
  auto outOfBounds = utils::string::match(rx, input, 3);
  // assert
  EXPECT_TRUE(fullMatch.has_value());
  EXPECT_EQ(fullMatch.value(), "user:42");
  EXPECT_TRUE(group1.has_value());
  EXPECT_EQ(group1.value(), "user");
  EXPECT_TRUE(group2.has_value());
  EXPECT_EQ(group2.value(), "42");
  EXPECT_FALSE(outOfBounds.has_value());
}

TEST(StringRegex, regex_noMatch) {
  // arrange
  std::regex rx(R"(\d+)");
  std::string input = "no_digits_here";
  // act
  auto result = utils::string::match(rx, input);
  // assert
  EXPECT_FALSE(result.has_value());
}

TEST(StringRegex, regex_stringLiteral) {
  // arrange
  std::string rx = R"(\d+)";
  std::string input = "qwe123asd";
  // act
  auto result = utils::string::match(rx, input);
  // assert
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "123");
}

TEST(StringRegex, replace) {
  // arrange
  std::regex rx(R"(\d+)");
  std::string input = "qwe123asd";
  // act
  auto result = utils::string::replace(rx, input, "###");
  // assert
  EXPECT_EQ(result, "qwe###asd");
};

TEST(StringRegex, replace_stringLiteral) {
  // arrange
  std::string rx = R"(\d+)";
  std::string input = "qwe123asd";
  // act
  auto result = utils::string::replace(rx, input, "###");
  // assert
  EXPECT_EQ(result, "qwe###asd");
};
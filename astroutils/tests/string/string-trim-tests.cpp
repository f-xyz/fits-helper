#include "string.hpp"
#include <gtest/gtest.h>

TEST(StringTrim, ltrim_whitespace) {
  // arrange
  std::string str = " \t\r\nhello world ";
  // act
  auto result = utils::string::ltrim(str);
  // assert
  EXPECT_EQ(result, "hello world ");
}

TEST(StringTrim, rtrim_whitespace) {
  // arrange
  std::string str = " hello world \t\r\n";
  // act
  auto result = utils::string::rtrim(str);
  // assert
  EXPECT_EQ(result, " hello world");
}

TEST(StringTrim, trim_bothSides) {
  // arrange
  std::string str = " \t\r\n hello world \r\n\t ";
  // act
  auto result = utils::string::trim(str);
  // assert
  EXPECT_EQ(result, "hello world");
}

TEST(StringTrim, trim_allWhitespace) {
  // arrange
  std::string str = " \t\r\n ";
  // act
  auto result = utils::string::trim(str);
  // assert
  EXPECT_EQ(result, "");
}

TEST(StringTrim, trim_emptyString) {
  // arrange
  std::string str = "";
  // act
  auto result = utils::string::trim(str);
  // assert
  EXPECT_EQ(result, "");
}
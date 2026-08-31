#include "string.hpp"
#include <gtest/gtest.h>

TEST(StringCase, uppercase_utf8) {
  // arrange
  std::string str = "Привіт";
  // act
  auto result = utils::string::uppercase(str);
  // assert
  EXPECT_EQ(result, "ПРИВІТ");
}

TEST(StringCase, lowercase_utf8) {
  // arrange
  std::string str = "Привіт";
  // act
  auto result = utils::string::lowercase(str);
  // assert
  EXPECT_EQ(result, "привіт");
}
#include "cli/colors.hpp"
#include "string.hpp"
#include <gtest/gtest.h>

TEST(StringFormat, formatNumber_positive) {
  // act
  auto result = utils::string::formatNumber(3.14159, 2);
  // assert
  EXPECT_EQ(result, utils::cli::rgb("3.14", 0x008000));
}

TEST(StringFormat, formatNumber_negative) {
  // act
  auto result = utils::string::formatNumber(-12.3456, 2);
  // assert
  EXPECT_EQ(result, utils::cli::rgb("-12.35", 0x800000));
}

TEST(StringFormat, formatNumber_zero) {
  // act
  auto result = utils::string::formatNumber(0.0, 2);
  // assert
  EXPECT_EQ(result, utils::cli::rgb("0.00", 0x888888));
}

TEST(StringFormat, formatNumber_customPrecision) {
  // act
  auto resultHighPrec = utils::string::formatNumber(1.23456, 4);
  auto resultZeroPrec = utils::string::formatNumber(42.0, 0);
  // assert
  EXPECT_EQ(resultHighPrec, utils::cli::rgb("1.2346", 0x008000));
  EXPECT_EQ(resultZeroPrec, utils::cli::rgb("42", 0x008000));
}
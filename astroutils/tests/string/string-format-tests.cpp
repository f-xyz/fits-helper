#include <astroutils/cli/colors.hpp>
#include <astroutils/string.hpp>

TEST(StringFormat, formatNumber_positive) {
  // act
  auto result = astroutils::string::format_number(3.14159, 2);
  // assert
  EXPECT_EQ(result, astroutils::cli::rgb("3.14", 0x008000));
}

TEST(StringFormat, formatNumber_negative) {
  // act
  auto result = astroutils::string::format_number(-12.3456, 2);
  // assert
  EXPECT_EQ(result, astroutils::cli::rgb("-12.35", 0x800000));
}

TEST(StringFormat, formatNumber_zero) {
  // act
  auto result = astroutils::string::format_number(0.0, 2);
  // assert
  EXPECT_EQ(result, astroutils::cli::rgb("0.00", 0x888888));
}

TEST(StringFormat, formatNumber_customPrecision) {
  // act
  auto resultHighPrec = astroutils::string::format_number(1.23456, 4);
  auto resultZeroPrec = astroutils::string::format_number(42.0, 0);
  // assert
  EXPECT_EQ(resultHighPrec, astroutils::cli::rgb("1.2346", 0x008000));
  EXPECT_EQ(resultZeroPrec, astroutils::cli::rgb("42", 0x008000));
}
#include <astroutils/cli/colors.hpp>
#include <astroutils/string.hpp>

using namespace std::chrono_literals;
using std::chrono::sys_days;

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

TEST(StringFormat, format_date) {
  // arrange
  const auto date = sys_days {2026y / 9 / 26} + 14h + 30min;
  // act
  const auto result = astroutils::string::format_date(date);
  // assert
  EXPECT_EQ(result, "2026-09-26T14:30:00Z");
}

TEST(StringFormat, format_date_customFormat) {
  // arrange
  const auto date = sys_days {2026y / 9 / 26} + 14h + 30min;
  const auto format = "%Y/%m/%d %H:%M:%S";
  // act
  const auto result = astroutils::string::format_date(date, format);
  // assert
  EXPECT_EQ(result, "2026/09/26 14:30:00");
}
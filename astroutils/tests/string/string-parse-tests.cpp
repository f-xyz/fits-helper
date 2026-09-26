#include <astroutils/string.hpp>

using namespace std::chrono_literals;
using std::chrono::sys_days;

TEST(StringParse, parse_date) {
  // arrange
  const auto date = "2026-09-26T14:30:00Z";
  // act
  const auto result = astroutils::string::parse_date(date);
  // assert
  EXPECT_EQ(*result, sys_days {2026y / 9 / 26} + 14h + 30min);
}

TEST(StringParse, parse_date_invalidInput) {
  // assert
  EXPECT_FALSE(astroutils::string::parse_date("not a date").has_value());
}

TEST(StringParse, parse_date_auto) {
  // arrange
  const std::vector<std::string> dates = {
    "2026-09-26T14:30:00Z",
    "2026-09-26T14:30:00",
    "2026-09-26 14:30:00",
    "2026-09-26",
    "Sat, 26 Sep 2026 14:30:00",
    "Sep 26, 2026",
  };

  const std::vector<std::chrono::sys_seconds> expected = {
    sys_days {2026y / 9 / 26} + 14h + 30min,
    sys_days {2026y / 9 / 26} + 14h + 30min,
    sys_days {2026y / 9 / 26} + 14h + 30min,
    sys_days {2026y / 9 / 26},
    sys_days {2026y / 9 / 26} + 14h + 30min,
    sys_days {2026y / 9 / 26}
  };

  for (std::size_t i = 0; i < dates.size(); ++i) {
    // act
    const auto result = astroutils::string::parse_date_auto(dates[i]);
    // assert
    EXPECT_EQ(*result, expected[i]);
  }
}

TEST(StringParse, parse_date_auto_invalidInput) {
  // assert
  EXPECT_FALSE(astroutils::string::parse_date_auto("not a date").has_value());
}

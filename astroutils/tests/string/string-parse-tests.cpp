#include <astroutils/string.hpp>

using std::chrono::hours;
using std::chrono::minutes;
using std::chrono::sys_days;
using std::chrono::year;

TEST(StringParse, parse_date) {
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
    sys_days {year {2026} / 9 / 26} + hours {14} + minutes {30},
    sys_days {year {2026} / 9 / 26} + hours {14} + minutes {30},
    sys_days {year {2026} / 9 / 26} + hours {14} + minutes {30},
    sys_days {year {2026} / 9 / 26},
    sys_days {year {2026} / 9 / 26} + hours {14} + minutes {30},
    sys_days {year {2026} / 9 / 26}
  };

  for (std::size_t i = 0; i < dates.size(); ++i) {
    // act
    const auto result = astroutils::string::parse_date(dates[i]);
    // assert
    EXPECT_EQ(*result, expected[i]);
  }
}

TEST(StringParse, parse_date_invalidInput) {
  // assert
  EXPECT_FALSE(astroutils::string::parse_date("not a date").has_value());
}

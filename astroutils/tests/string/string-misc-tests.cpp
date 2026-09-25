#include <astroutils/string.hpp>

TEST(StringMisc, quote) {
  // arrange
  std::string str = "hello";
  // act
  auto result = astroutils::string::quote(str);
  // assert
  EXPECT_EQ(result, "\"hello\"");
}

TEST(StringMisc, quote_custom) {
  // arrange
  std::string str = "hello";
  // act
  auto result = astroutils::string::quote(str, "@@@");
  // assert
  EXPECT_EQ(result, "@@@hello@@@");
}

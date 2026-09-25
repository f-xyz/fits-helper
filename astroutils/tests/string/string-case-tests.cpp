#include <astroutils/string.hpp>

TEST(StringCase, uppercase_utf8) {
  // arrange
  std::string str = "Привіт";
  // act
  auto result = astroutils::string::uppercase(str);
  // assert
  EXPECT_EQ(result, "ПРИВІТ");
}

TEST(StringCase, lowercase_utf8) {
  // arrange
  std::string str = "Привіт";
  // act
  auto result = astroutils::string::lowercase(str);
  // assert
  EXPECT_EQ(result, "привіт");
}
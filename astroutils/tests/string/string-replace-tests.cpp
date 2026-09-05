#include "string.hpp"

TEST(StringReplace, replace_all) {
  // arrange
  std::string input = "qwe123asd123zxc";
  // act
  auto result = utils::string::replace_all(input, "123", "!@#");
  // assert
  EXPECT_EQ(result, "qwe!@#asd!@#zxc");
}

////////////////////////////////////////
// Generated ///////////////////////////
////////////////////////////////////////

TEST(StringReplace, BasicAndBoundaries) {
  EXPECT_EQ(utils::string::replace_all("hello world", "world", "there"),
            "hello there");
  EXPECT_EQ(utils::string::replace_all("cat cat cat", "cat", "dog"),
            "dog dog dog");
  EXPECT_EQ(utils::string::replace_all("prefix_middle_suffix", "prefix_", ""),
            "middle_suffix");
  EXPECT_EQ(utils::string::replace_all("prefix_middle_suffix", "_suffix", ""),
            "prefix_middle");
}

TEST(StringReplace, EdgeCases) {
  EXPECT_EQ(utils::string::replace_all("", "a", "b"), "");
  EXPECT_EQ(utils::string::replace_all("abc", "", "xyz"),
            "abc"); // Empty find guard
  EXPECT_EQ(utils::string::replace_all("abc", "d", "e"), "abc"); // No match
  EXPECT_EQ(utils::string::replace_all("a", "abc", "def"),
            "a"); // Find longer than input
}

TEST(StringReplace, RecursivePatternSafety) {
  // Replacement contains search pattern
  EXPECT_EQ(utils::string::replace_all("banana", "a", "ba"), "bbanbanba");
  // Shrinking and growing
  EXPECT_EQ(utils::string::replace_all("aaaa", "aa", "b"), "bb");
  EXPECT_EQ(utils::string::replace_all("a", "a", "aaaa"), "aaaa");
}

TEST(StringReplace, UnicodeAndSpecialChars) {
  EXPECT_EQ(utils::string::replace_all("line1\r\nline2", "\r\n", "\n"),
            "line1\nline2");
  EXPECT_EQ(utils::string::replace_all("Hello 🌍", "🌍", "Universe"),
            "Hello Universe");
}
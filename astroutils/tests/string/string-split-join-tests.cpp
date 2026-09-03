#include "string.hpp"

using testing::ElementsAre;

TEST(StringSplit, split) {
  // arrange
  std::string str = "qwe,asd,zxc";
  // act
  auto result = utils::string::split(str, ",");
  // assert
  EXPECT_THAT(result, ElementsAre("qwe", "asd", "zxc"));
}

TEST(StringSplit, split_emptyDelimiter) {
  // arrange
  std::string str = "qwe";
  // act
  auto result = utils::string::split(str, "");
  // assert
  EXPECT_THAT(result, ElementsAre("q", "w", "e"));
}

TEST(StringSplit, split_delimiterNotFound) {
  // arrange
  std::string str = "qwe";
  // act
  auto result = utils::string::split(str, ",");
  // assert
  EXPECT_THAT(result, ElementsAre("qwe"));
}

TEST(StringJoin, join) {
  // arrange
  std::vector<std::string> strings = {"qwe", "asd", "zxc"};
  // act
  auto result = utils::string::join(strings, ",");
  // assert
  EXPECT_EQ(result, "qwe,asd,zxc");
}

TEST(StringJoin, join_emptyVector) {
  // arrange
  std::vector<std::string> strings = {};
  // act
  auto result = utils::string::join(strings, ",");
  // assert
  EXPECT_EQ(result, "");
}
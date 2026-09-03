#include "cli/colors.hpp"

using ::testing::Eq;

TEST(CliColors, Constants) {
  // assert
  EXPECT_THAT(utils::cli::constants::RESET, Eq("\033[0m"));
  EXPECT_THAT(utils::cli::constants::BOLD, Eq("\033[1m"));
  EXPECT_THAT(utils::cli::constants::RED, Eq("\033[31m"));
}

TEST(CliColors, RgbWithChannels) {
  // act
  const auto result = utils::cli::rgb("x", 1, 2, 3);
  // assert
  EXPECT_THAT(result, Eq("\033[38;2;1;2;3mx\033[0m"));
}

TEST(CliColors, RgbWithPackedColor) {
  // act
  const auto result = utils::cli::rgb("x", 0x010203);
  // assert
  EXPECT_THAT(result, Eq("\033[38;2;1;2;3mx\033[0m"));
}

TEST(CliColors, Bold) {
  // act
  const auto result = utils::cli::bold("x");
  // assert
  EXPECT_THAT(result, Eq("\033[1mx\033[0m"));
}

TEST(CliColors, RemoveColors) {
  // act
  const auto result = utils::cli::removeColors("\033[1mhello\033[0m");
  // assert
  EXPECT_THAT(result, Eq("hello"));
}

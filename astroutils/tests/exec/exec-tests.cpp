#include "exec.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Eq;

TEST(Exec, CapturesCommandOutputAndExitCode) {
  // arrange
  const auto command = "printf hello";
  // act
  const auto result = utils::process::exec(command);
  // assert
  EXPECT_THAT(result.output, Eq("hello"));
  EXPECT_EQ(result.code, 0);
}

TEST(Exec, PreservesMultilineOutput) {
  // arrange
  const auto command = "printf '%b' 'a\\nb\\n'";
  // act
  const auto result = utils::process::exec(command);
  // assert
  EXPECT_THAT(result.output, Eq("a\nb\n"));
  EXPECT_EQ(result.code, 0);
}

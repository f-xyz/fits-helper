#include "exec.hpp"
#include <cerrno>
#include <gmock/gmock.h>
#include <vector>
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

TEST(Exec, CallbackReceivesEachLine) {
  // arrange
  std::vector<std::string> lines;
  auto callback = [&lines](const char *data) { lines.emplace_back(data); };
  const auto command = "printf '%b' 'line1\\nline2\\n'";
  // act
  const auto result = utils::process::exec(command, callback);
  // assert result
  EXPECT_EQ(result.output, "line1\nline2\n");
  EXPECT_EQ(result.code, 0);
  // assert callback
  EXPECT_EQ(lines.size(), 2);
  EXPECT_EQ(lines[0], "line1\n");
  EXPECT_EQ(lines[1], "line2\n");
}

#include "Logger.hpp"
#include "fs.hpp"

using ::testing::EndsWith;
using ::testing::HasSubstr;
using ::testing::StartsWith;

class LoggerTests : public testing::Test {
protected:
  std::string path;
  virtual void SetUp() override { path = utils::fs::tmpFile(); }
  virtual void TearDown() override { /* std::filesystem::remove(path); */ }
};

TEST_F(LoggerTests, InfoWritesTimestampedLineToFile) {
  // arrange
  utils::logging::Logger logger(path);
  // act
  logger.info("hello {}", 42);
  // assert
  const auto log = utils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] INFO hello 42\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}

TEST_F(LoggerTests, HeaderWritesInfoLineToFile) {
  // arrange
  utils::logging::Logger logger(path);
  // act
  logger.header("section {}", 1);
  // assert
  const auto log = utils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] INFO section 1\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}

TEST_F(LoggerTests, DebugWritesDebugLineToFile) {
  // arrange
  utils::logging::Logger logger(path);
  // act
  logger.debug("value {}", 7);
  // assert
  const auto log = utils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] DEBUG value 7\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}

TEST_F(LoggerTests, SuccessWritesInfoLineToFile) {
  // arrange
  utils::logging::Logger logger(path);
  // act
  logger.success("done {}", 1);
  // assert
  const auto log = utils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] INFO done 1\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}

TEST_F(LoggerTests, ErrorWritesSeverityAndStripsColors) {
  // arrange
  utils::logging::Logger logger(path);
  // act
  logger.error("{}", utils::cli::bold("boom"));
  // assert
  const auto log = utils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] ERROR boom\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}
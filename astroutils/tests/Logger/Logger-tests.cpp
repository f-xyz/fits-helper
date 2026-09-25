#include <astroutils/Logger.hpp>
#include <astroutils/fs.hpp>

using ::testing::EndsWith;
using ::testing::HasSubstr;
using ::testing::StartsWith;

class LoggerTests : public testing::Test {
protected:
  std::string path;
  virtual void SetUp() override { path = astroutils::fs::tmpFile(); }
  virtual void TearDown() override { /* std::filesystem::remove(path); */ }
};

TEST_F(LoggerTests, InfoWritesTimestampedLineToFile) {
  // arrange
  astroutils::logging::Logger logger(path);
  // act
  logger.info("hello {}", 42);
  // assert
  const auto log = astroutils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] INFO hello 42\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}

TEST_F(LoggerTests, HeaderWritesInfoLineToFile) {
  // arrange
  astroutils::logging::Logger logger(path);
  // act
  logger.header("section {}", 1);
  // assert
  const auto log = astroutils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] INFO section 1\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}

TEST_F(LoggerTests, DebugWritesDebugLineToFile) {
  // arrange
  astroutils::logging::Logger logger(path);
  // act
  logger.debug("value {}", 7);
  // assert
  const auto log = astroutils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] DEBUG value 7\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}

TEST_F(LoggerTests, SuccessWritesInfoLineToFile) {
  // arrange
  astroutils::logging::Logger logger(path);
  // act
  logger.success("done {}", 1);
  // assert
  const auto log = astroutils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] INFO done 1\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}

TEST_F(LoggerTests, ErrorWritesSeverityAndStripsColors) {
  // arrange
  astroutils::logging::Logger logger(path);
  // act
  logger.error("{}", astroutils::cli::bold("boom"));
  // assert
  const auto log = astroutils::fs::readText(path);
  EXPECT_THAT(log, StartsWith("["));
  EXPECT_THAT(log, HasSubstr("] ERROR boom\n"));
  EXPECT_THAT(log, EndsWith("\n"));
}
#include "fs.hpp"
#include <filesystem>

class FS : public testing::Test {
protected:
  std::string tmpDir;

  virtual void SetUp() override {
    tmpDir = std::filesystem::temp_directory_path().string() + "/astroutils";
    std::filesystem::create_directory(tmpDir);
  }

  virtual void TearDown() override {
    std::filesystem::remove_all(tmpDir);
  }

  std::string getTmpFile(const std::string &name) {
    return tmpDir + "/" + name;
  }
};

TEST_F(FS, readDir) {
  // arrange
  utils::fs::writeFile(tmpDir + "/1", "1");
  utils::fs::writeFile(tmpDir + "/2", "2");
  // act
  const auto result = utils::fs::readDir(tmpDir);
  // assert
  EXPECT_THAT(result, testing::ElementsAre(tmpDir + "/1", tmpDir + "/2"));
};

TEST_F(FS, readDir_nonExistentDir) {
  // act
  const auto result = utils::fs::readDir(tmpDir + "/qwe");
  // assert
  EXPECT_THAT(result, testing::IsEmpty());
};

TEST_F(FS, writeFile_nonexisting) {
  // arrange
  std::string nonExistentFile = tmpDir + "/qqq/www/eee.txt";
  std::vector<int> ints = {1, 2, 3, 4, 5};
  // act
  // assert
  EXPECT_THROW(utils::fs::writeFile(nonExistentFile, ints),
               std::filesystem::filesystem_error);
};

TEST_F(FS, readFile_nonexisting) {
  // arrange
  std::string file = tmpDir + "/qqq/www/eee.txt";
  // act
  // assert
  EXPECT_THROW(utils::fs::readFile(file),
               std::filesystem::filesystem_error);
};

TEST_F(FS, writeFile_readFile_with_integers) {
  // arrange
  std::string file = tmpDir + "/ints.txt";
  std::vector<int> ints = {1, 2, 3, 4, 5};
  // act
  utils::fs::writeFile(file, ints).close();
  auto result = utils::fs::readFile<int>(file);
  // assert
  EXPECT_EQ(result, ints);
};

TEST_F(FS, writeFile_readText) {
  // arrange
  std::string file = tmpDir + "/text.txt";
  std::string text = "Hello!";
  // act
  utils::fs::writeFile(file, text);
  auto result = utils::fs::readText(file);
  // assert
  EXPECT_EQ(result, text);
};

TEST_F(FS, tmpFileEmptyDirectory) {
  // act
  auto result = utils::fs::tmpFile(tmpDir);
  // assert
  EXPECT_EQ(result, tmpDir + "/1");
};

TEST_F(FS, tmpFileNonEmptyDirectory) {
  // arrange
  utils::fs::writeFile(tmpDir + "/123", "1");
  // act
  auto result = utils::fs::tmpFile(tmpDir);
  // assert
  EXPECT_EQ(result, tmpDir + "/124");
};

TEST_F(FS, tmpFileWithNoArguments) {
  // arrange
  auto tmp = std::filesystem::temp_directory_path().string();
  // act
  auto result = utils::fs::tmpFile();
  // assert
  EXPECT_THAT(result, testing::MatchesRegex(tmp + "/[0-9]+"));
};
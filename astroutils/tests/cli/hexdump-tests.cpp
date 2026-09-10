#include "cli/hexdump.hpp"

using ::testing::HasSubstr;

TEST(CliHexdump, EmptySpan) {
  // arrange
  std::span<const std::byte> empty;
  // act
  auto result = utils::cli::hexdump(empty);
  // assert
  EXPECT_TRUE(result.empty());
}

TEST(CliHexdump, SimpleData) {
  std::vector<std::byte> data = {std::byte{0x00}, std::byte{0x01},
                                 std::byte{'A'}, std::byte{0xFF}};
  std::string out = utils::cli::hexdump(std::span<const std::byte>(data));
  // Should contain address offset and printable ASCII for 'A'
  EXPECT_THAT(out, HasSubstr("0000"));
  EXPECT_THAT(out, HasSubstr("A"));
  // Non‑printable bytes should be represented as '.'
  EXPECT_THAT(out, HasSubstr("."));
}

#pragma once

#include <string>

/*
ANSI Escape Codes:
  - The core syntax for a standard 16-color ANSI code follows this structure: `\033[Style;ColorCode m`.
  - Styles: 0 (Normal/Reset), 1 (Bold/Bright), 4 (Underline).
  - Foreground (30-37): 31 (Red), 32 (Green), 33 (Yellow), 34 (Blue).
  - Background (40-47): 41 (Red), 42 (Green), 43 (Yellow), 44 (Blue).
  - Example: a Bold Red Text on a Yellow Background: `\033[1;31;43m`.

Advanced Colors (256-Color & True Color RGB):
  - Modern terminals allow a larger spectrum using specific extended formatting structures.
  - 256 Colors: Format via `\033[38;5;[0-255]m` (Foreground) or `\033[48;5;[0-255]m` (Background).
  - True Color (RGB): Format via `\033[38;2;R;G;Bm` (where R, G, B are integers from 0 to 255).
*/

namespace utils::cli {

namespace constants {

inline constexpr const char *RESET = "\033[0m";
inline constexpr const char *BLACK = "\033[30m";
inline constexpr const char *RED = "\033[31m";
inline constexpr const char *GREEN = "\033[32m";
inline constexpr const char *YELLOW = "\033[33m";
inline constexpr const char *BLUE = "\033[34m";
inline constexpr const char *MAGENTA = "\033[35m";
inline constexpr const char *CYAN = "\033[36m";
inline constexpr const char *WHITE = "\033[37m";
inline constexpr const char *BOLD = "\033[1m";
inline constexpr const char *BOLD_BLACK = "\033[1m\033[30m";
inline constexpr const char *BOLD_RED = "\033[1m\033[31m";
inline constexpr const char *BOLD_GREEN = "\033[1m\033[32m";
inline constexpr const char *BOLD_YELLOW = "\033[1m\033[33m";
inline constexpr const char *BOLD_BLUE = "\033[1m\033[34m";
inline constexpr const char *BOLD_MAGENTA = "\033[1m\033[35m";
inline constexpr const char *BOLD_CYAN = "\033[1m\033[36m";
inline constexpr const char *BOLD_WHITE = "\033[1m\033[37m";

} // namespace constants

using byte = unsigned char;

std::string rgb(const std::string &s, byte r, byte g, byte b);
std::string rgb(const std::string &s, const unsigned int color);
std::string bold(const std::string &s);
std::string removeColors(const std::string &s);

} // namespace utils::cli
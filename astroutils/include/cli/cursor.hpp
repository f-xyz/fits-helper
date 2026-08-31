#pragma once

/*
\033[F : Moves cursor up to the beginning of the previous line.
\033[A : Moves cursor up one line (keeps horizontal position).
\033[H : Moves cursor to the top-left corner (home position).
\033[K : Clears the line from the cursor position to the end of the line.
\033[2J : Clears the entire screen.
*/

namespace utils::cursor {

namespace constants {

inline constexpr const char *MOVE_LEFT = "\033[F";
inline constexpr const char *MOVE_UP = "\033[A";
inline constexpr const char *MOVE_HOME = "\033[H";
inline constexpr const char *CLEAR_LINE = "\033[K";
inline constexpr const char *CLEAR_SCREEN = "\033[2J";

} // namespace constants

} // namespace utils::cursor
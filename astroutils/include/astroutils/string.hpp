#pragma once

#include <chrono>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace astroutils::string {

////////////////////////////////////////
// Splitting and Joining ///////////////
////////////////////////////////////////

std::vector<std::string> split(std::string_view string,
                               std::string_view delimiter);

std::string join(const std::vector<std::string> &array,
                 std::string_view delimiter);

////////////////////////////////////////
// Regex ///////////////////////////////
////////////////////////////////////////

std::optional<std::string> regex(const std::regex &regex,
                                 std::string_view string,
                                 const std::size_t nMatch = 0);

std::optional<std::string> regex(std::string_view regex,
                                 std::string_view string,
                                 const std::size_t nMatch = 0);

std::string regex_replace(const std::regex &regex, std::string_view string,
                          std::string_view replacement);

std::string regex_replace(std::string_view regex, std::string_view string,
                          std::string_view replacement);

////////////////////////////////////////
// Replacing ///////////////////////////
////////////////////////////////////////

std::string replace_all(std::string string, std::string_view find,
                        std::string_view replacement);

////////////////////////////////////////
// Slicing /////////////////////////////
////////////////////////////////////////

std::string slice(std::string_view string, int start);
std::string slice(std::string_view string, int start, int end);

////////////////////////////////////////
// Trimmming ///////////////////////////
////////////////////////////////////////

std::string ltrim(std::string_view string);
std::string rtrim(std::string_view string);
std::string trim(std::string_view string);

////////////////////////////////////////
// Case Conversion /////////////////////
////////////////////////////////////////

std::string uppercase(std::string_view string);
std::string lowercase(std::string_view string);

////////////////////////////////////////
// Miscellaneous ///////////////////////
////////////////////////////////////////

std::string quote(std::string_view string, std::string_view wrapper = "\"");

////////////////////////////////////////
// Formatting //////////////////////////
////////////////////////////////////////

std::string format_number(double number, int precision = 2);
std::string format_date(const std::chrono::sys_seconds &date,
                        std::string_view format = "%Y-%m-%dT%H:%M:%SZ");

////////////////////////////////////////
// Parsing /////////////////////////////
////////////////////////////////////////

using OptionalSeconds = std::optional<std::chrono::sys_seconds>;

OptionalSeconds parse_date(std::string_view string,
                           std::string_view format = "%Y-%m-%dT%H:%M:%SZ");

OptionalSeconds parse_date_auto(std::string_view string);

} // namespace astroutils::string

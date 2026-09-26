#pragma once

#include <chrono>
#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace astroutils::string {

////////////////////////////////////////
// Splitting and Joining ///////////////
////////////////////////////////////////

std::vector<std::string> split(const std::string &string,
                               const std::string &delimiter);

std::string join(const std::vector<std::string> &array,
                 const std::string &delimiter);

////////////////////////////////////////
// Regex ///////////////////////////////
////////////////////////////////////////

std::optional<std::string> regex(const std::regex &regex,
                                 const std::string &string,
                                 const std::size_t nMatch = 0);

std::optional<std::string> regex(const std::string &regex,
                                 const std::string &string,
                                 const std::size_t nMatch = 0);

std::string regex_replace(const std::regex &regex, const std::string &string,
                          const std::string &replacement);

std::string regex_replace(const std::string &regex, const std::string &string,
                          const std::string &replacement);

////////////////////////////////////////
// Replacing ///////////////////////////
////////////////////////////////////////

std::string replace_all(std::string string, const std::string &find,
                        const std::string &replacement);

////////////////////////////////////////
// Slicing /////////////////////////////
////////////////////////////////////////

std::string slice(const std::string &string, int start);
std::string slice(const std::string &string, int start, int end);

////////////////////////////////////////
// Trimmming ///////////////////////////
////////////////////////////////////////

std::string ltrim(const std::string &string);
std::string rtrim(const std::string &string);
std::string trim(const std::string &string);

////////////////////////////////////////
// Case Conversion /////////////////////
////////////////////////////////////////

std::string uppercase(const std::string &string);
std::string lowercase(const std::string &string);

////////////////////////////////////////
// Miscellaneous ///////////////////////
////////////////////////////////////////

std::string quote(const std::string &string, const std::string &wrapper = "\"");

////////////////////////////////////////
// Formatting //////////////////////////
////////////////////////////////////////

std::string format_number(double number, int precision = 2);
std::string format_date(const std::chrono::sys_seconds &date,
                        const std::string &format = "%Y-%m-%dT%H:%M:%SZ");

////////////////////////////////////////
// Parsing /////////////////////////////
////////////////////////////////////////

using OptionalSeconds = std::optional<std::chrono::sys_seconds>;

OptionalSeconds parse_date(const std::string &string,
                           const std::string &format = "%Y-%m-%dT%H:%M:%SZ");

OptionalSeconds parse_date_auto(const std::string &string);

} // namespace astroutils::string

#pragma once

namespace utils::string {

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
// Formatting //////////////////////////
////////////////////////////////////////

std::string formatNumber(double number, int precision = 2);

} // namespace utils::string

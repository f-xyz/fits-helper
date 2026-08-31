#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace utils::string {

////////////////////////////////////////
// Splitting and Joining ///////////////
////////////////////////////////////////

std::vector<std::string> split(const std::string &str,
                               const std::string &delimiter);

std::string join(const std::vector<std::string> &arr,
                 const std::string &delimiter);

////////////////////////////////////////
// Regex ///////////////////////////////
////////////////////////////////////////

std::optional<std::string>
regex(const std::regex &regex, const std::string &str, std::size_t nMatch = 0);

std::optional<std::string>
regex(const std::string &regex, const std::string &str, std::size_t nMatch = 0);

////////////////////////////////////////
// Slicing /////////////////////////////
////////////////////////////////////////

std::string slice(const std::string &str, int start);
std::string slice(const std::string &str, int start, int end);

////////////////////////////////////////
// Trimmming ///////////////////////////
////////////////////////////////////////

std::string ltrim(const std::string &str);
std::string rtrim(const std::string &str);
std::string trim(const std::string &str);

////////////////////////////////////////
// Case Conversion /////////////////////
////////////////////////////////////////

std::string uppercase(const std::string &str);
std::string lowercase(const std::string &str);

////////////////////////////////////////
// Formatting //////////////////////////
////////////////////////////////////////

std::string formatNumber(double x, int precision = 2);

}; // namespace utils::string

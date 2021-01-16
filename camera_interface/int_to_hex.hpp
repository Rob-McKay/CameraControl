#pragma once

#include <iomanip>
#include <sstream>
#include <string>

/// Convert an integer to a string in hex format
/// The minimum width is padded with leading zeros; if not specified, this width is taken from the
/// type of the argument. This function is suitable from char to long long. Pointers, floating point
/// numbers, etc. are not supported, their transfer leads to an (intentional!) compiler error.
template <typename T> inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
    return ss.str();
}
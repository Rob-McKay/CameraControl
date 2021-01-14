#pragma once

#include <regex>
#include <string>

inline std::regex convert_file_wildcards_to_regex(std::string file_pattern)
{
    std::string pattern;
    for (auto c : file_pattern)
    {
        switch (c)
        {
        default:
            pattern += c;
            break;
        case '.':
            pattern += "\\.";
            break;
        case '\\':
            pattern += "\\\\";
            break;
        case '?':
            pattern += ".";
            break;
        case '*':
            pattern += ".*";
            break;
        }
    }

    return std::regex(pattern, std::regex_constants::ECMAScript | std::regex_constants::icase);
}

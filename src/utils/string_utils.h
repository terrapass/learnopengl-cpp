#pragma once

#include <string>
#include <vector>

//
// Constants
//

const std::string COMMA_SEPARATOR = ", ";

//
// Utilities
//

inline std::string MakeCommaSeparatedListFromPack()
{
    return "";
}

inline const std::string & MakeCommaSeparatedListFromPack(const std::string & singleString)
{
    return singleString;
}

template <typename HeadString, typename... TailStrings>
inline std::string MakeCommaSeparatedListFromPack(HeadString && headString, TailStrings &&... tailStrings)
{
    return headString + COMMA_SEPARATOR + MakeCommaSeparatedListFromPack(std::forward<TailStrings>(tailStrings)...);
}

std::string MakeCommaSeparatedList(const std::vector<std::string> & strings);

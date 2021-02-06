#pragma once

#include <string>

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
    static const std::string SEPARATOR = ", ";

    return headString + SEPARATOR + MakeCommaSeparatedListFromPack(std::forward<TailStrings>(tailStrings)...);
}

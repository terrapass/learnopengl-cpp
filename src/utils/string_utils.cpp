#include "string_utils.h"

//
// Utilities
//

std::string MakeCommaSeparatedList(const std::vector<std::string> & strings)
{
    std::string result;

    bool mustAddSeparator = false;
    for (const std::string & s : strings)
    {
        if (mustAddSeparator)
            result += COMMA_SEPARATOR;

        result += s;

        mustAddSeparator = true;
    }

    return result;
}

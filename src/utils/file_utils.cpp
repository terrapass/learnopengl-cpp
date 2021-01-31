#include "file_utils.h"

#include <fstream>
#include <sstream>

//
// Utilities
//

std::string ReadFileContent(const std::string & path)
{
    std::ifstream fin(path);
    if (fin.fail())
        throw FileException(path);

    std::stringstream sout;
    sout<< fin.rdbuf();

    return sout.str();
}

//
// Exceptions
//

FileException::FileException(const std::string & path):
    std::runtime_error("Failed to open file " + path)
{
    // Empty
}

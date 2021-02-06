#include "file_utils.h"

#include <fstream>
#include <sstream>
#include <filesystem>

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

std::string GetFileExtension(const std::string & path)
{
    const size_t lastDotIdx = path.rfind('.');
    if (lastDotIdx == std::string::npos)
        return "";

    return path.substr(lastDotIdx + 1);
}

bool DoesFileExist(const std::string & path)
{
    return std::filesystem::exists(path);
}

//
// Exceptions
//

FileException::FileException(const std::string & path):
    std::runtime_error("Failed to open file " + path)
{
    // Empty
}

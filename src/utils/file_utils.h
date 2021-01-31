#pragma once

#include <string>
#include <stdexcept>

//
// Utilities
//

std::string ReadFileContent(const std::string & path);

//
// Exceptions
//

class FileException final: public std::runtime_error
{
public: // Construction

    FileException(const std::string & path);
};

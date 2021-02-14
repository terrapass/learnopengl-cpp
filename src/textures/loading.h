#pragma once

#include <stdexcept>

#include "TextureData.h"

//
// Utilities
//

TextureData LoadTextureDataFromFile(const std::string & textureFilename);

//
// Exceptions
//

class TextureLoadingException final: public std::runtime_error
{
public: // Construction

    explicit TextureLoadingException(const std::string & textureFilename);
};

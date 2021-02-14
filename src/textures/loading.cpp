#include "loading.h"

#include <cassert>

#include "stb_image.h"

#include "config.h"
#include "logging.h"

//
// Forward declarations
//

static std::string GetFullTexturePath(const std::string & textureFilename);

//
// Utilities
//

TextureData LoadTextureDataFromFile(const std::string & textureFilename)
{
    TextureMetadata metadata;
    metadata.SourceName = textureFilename;

    TextureByte * const data = stbi_load(
        GetFullTexturePath(textureFilename).c_str(),
        &metadata.Width,
        &metadata.Height,
        &metadata.ChannelsCount,
        0
    );

    if (data == nullptr)
        throw TextureLoadingException(textureFilename);

    BOOST_LOG_TRIVIAL(debug)<< "Loaded texture data from " << textureFilename;

    return TextureData::CreateFromStbImage(data, std::move(metadata));
}

//
// Service
//

static std::string GetFullTexturePath(const std::string & textureFilename)
{
    assert(TEXTURES_DIR.back() == '/' || TEXTURES_DIR.empty());
    assert(!textureFilename.empty());

    return TEXTURES_DIR + textureFilename;
}

//
// Exceptions
//

TextureLoadingException::TextureLoadingException(const std::string & textureFilename):
    std::runtime_error("Failed to load texture data from file " + textureFilename)
{
    // Empty
}

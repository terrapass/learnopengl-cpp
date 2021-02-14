#pragma once

#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <optional>

#include <stb_image.h>

//
// Interface types
//

using TextureByte = unsigned char;

struct TextureMetadata final
{
    int         Width;
    int         Height;
    int         ChannelsCount;
    std::string SourceName;
};

//
// TextureData
//

class TextureData final
{
private: // Service types

    using TextureDataStorage = std::variant<
        std::vector<TextureByte>,
        std::unique_ptr<TextureByte, decltype(&stbi_image_free)>
    >;

private: // Construction

    TextureData(TextureDataStorage && storage, TextureMetadata metadata);

public: // Copy / Move

    TextureData(const TextureData &) = delete;

    TextureData(TextureData &&);

    TextureData & operator=(const TextureData &) = delete;

    TextureData & operator=(TextureData &&);

public: // Factory methods

    static TextureData CreateFromBytes(std::vector<TextureByte> bytes, TextureMetadata metadata);

    static TextureData CreateFromStbImage(TextureByte * data, TextureMetadata metadata);

public: // Interface

    const TextureByte * GetData() const;

    size_t GetDataSize() const;

    const TextureMetadata & GetMetadata() const;

private: // Members

    std::optional<TextureDataStorage> m_Storage;
    TextureMetadata                   m_Metadata;
};

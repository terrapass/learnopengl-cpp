#include "TextureData.h"

#include <cassert>

//
// Construction
//

TextureData::TextureData(TextureDataStorage && storage, TextureMetadata metadata):
    m_Storage (std::move(storage)),
    m_Metadata(std::move(metadata))
{
    // Empty
}

//
// Copy / Move
//

TextureData::TextureData(TextureData && other):
    m_Storage (std::move(other.m_Storage)),
    m_Metadata(std::move(other.m_Metadata))
{
    other.m_Storage.reset();
}

TextureData & TextureData::operator=(TextureData && other)
{
    m_Storage  = std::move(other.m_Storage);
    m_Metadata = std::move(other.m_Metadata);

    other.m_Storage.reset();

    return *this;
}

//
// Factory methods
//

TextureData TextureData::CreateFromBytes(std::vector<TextureByte> bytes, TextureMetadata metadata)
{
    return TextureData(std::move(bytes), std::move(metadata));
}

TextureData TextureData::CreateFromStbImage(TextureByte * data, TextureMetadata metadata)
{
    return TextureData(
        std::unique_ptr<TextureByte, decltype(&stbi_image_free)>(data, &stbi_image_free),
        std::move(metadata)
    );
}

//
// Interface
//

const TextureByte * TextureData::GetData() const
{
    assert(m_Storage.has_value() && "storage must be initialized and not yet moved from");

    if (std::holds_alternative<std::vector<TextureByte>>(*m_Storage))
        return std::get<std::vector<TextureByte>>(*m_Storage).data();

    if (std::holds_alternative<std::unique_ptr<TextureByte, decltype(&stbi_image_free)>>(*m_Storage))
        return std::get<std::unique_ptr<TextureByte, decltype(&stbi_image_free)>>(*m_Storage).get();

    assert(false && "unrecognized texture data storage type");
    return nullptr;
}

size_t TextureData::GetDataSize() const
{
    assert(m_Storage.has_value() && "storage must be initialized and not yet moved from");

    if (std::holds_alternative<std::vector<TextureByte>>(*m_Storage))
        return std::get<std::vector<TextureByte>>(*m_Storage).size();

    if (std::holds_alternative<std::unique_ptr<TextureByte, decltype(&stbi_image_free)>>(*m_Storage))
        return m_Metadata.Width * m_Metadata.Height * m_Metadata.ChannelsCount;

    assert(false && "unrecognized texture data storage type");
    return static_cast<size_t>(-1);
}

const TextureMetadata & TextureData::GetMetadata() const
{
    return m_Metadata;
}

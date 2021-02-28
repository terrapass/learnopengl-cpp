#include "construction.h"

#include <array>
#include <vector>

#include "gl/utils.h"
#include "utils/collection_utils.h"

//
// Service
//

namespace
{

struct Vertex final
{
public: // Constants

    static const size_t FLOATS_PER_VERTEX = 8;

public: // Attributes

    glm::vec3 Position;
    glm::vec3 TintRgb;
    glm::vec2 TextureUv;

private: // Constants

    static const glm::vec3 NO_TINT_RGB;

public: // Construction

    Vertex():
        Position (0.0f),
        TintRgb  (NO_TINT_RGB),
        TextureUv(0.0f)
    {
        // Empty
    }

    std::array<float, FLOATS_PER_VERTEX> AsVertexData() const
    {
        return std::array{
            Position.x, Position.y, Position.z,
            TintRgb.x, TintRgb.y, TintRgb.z,
            TextureUv.x, TextureUv.y
        };
    }
};

const glm::vec3 Vertex::NO_TINT_RGB = glm::vec3(1.0f, 1.0f, 1.0f);

} // anonymous namespace

static std::vector<float> VerticesToVertexData(const std::vector<Vertex> & vertices)
{
    std::vector<float> result;
    result.reserve(vertices.size() * Vertex::FLOATS_PER_VERTEX);

    for (const Vertex & vertex : vertices)
    {
        const auto vertexData = vertex.AsVertexData();

        result.insert(result.cend(), vertexData.cbegin(), vertexData.cend());
    }

    return result;
}

static UniqueVao MakeIndexedVao(const std::vector<Vertex> & vertices, const std::vector<GLuint> & indices)
{
    const GLuint oldVertexArray = GetBoundVertexArray();
    const GLuint oldArrayBuffer = GetBoundArrayBuffer();

    const std::vector<float> vertexData = VerticesToVertexData(vertices);

    UniqueVao vertexArrayObject = UniqueVao::Create();

    std::vector<UniqueBuffer> bufferObjects = UniqueBuffer::CreateMany(2);

    const UniqueBuffer vertexBufferObject  = std::move(bufferObjects[0]);
    const UniqueBuffer elementBufferObject = std::move(bufferObjects[1]);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, SizeOfCollectionData(vertexData), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, SizeOfCollectionData(indices), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::FLOATS_PER_VERTEX*sizeof(float), (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::FLOATS_PER_VERTEX*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Vertex::FLOATS_PER_VERTEX*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(oldVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, oldArrayBuffer);

    return vertexArrayObject;
}

//
// Utilities
//

MeshData CreateAabbMesh(const glm::vec3 & minCoords, const glm::vec3 & maxCoords, const bool mustUseAxisTint)
{
    static const size_t VERTICES_COUNT = 8;

    std::vector<Vertex> vertices(VERTICES_COUNT);

    for (size_t i = 0; i < VERTICES_COUNT; i++)
    {
        const bool useMinX = (i & (1 << 0)) == 0;
        const bool useMinY = (i & (1 << 1)) == 0;
        const bool useMinZ = (i & (1 << 2)) == 0;

        Vertex & vertex = vertices[i];

        vertex.Position = glm::vec3(
            (useMinX ? minCoords : maxCoords).x,
            (useMinY ? minCoords : maxCoords).y,
            (useMinZ ? minCoords : maxCoords).z
        );

        if (mustUseAxisTint)
        {
            vertex.TintRgb = glm::vec3(
                useMinX ? 0.0f : 1.0f,
                useMinY ? 0.0f : 1.0f,
                useMinZ ? 0.0f : 1.0f
            );
        }

        vertex.TextureUv = glm::vec2(
            useMinX ? 0.0f : 1.0f,
            useMinY ? 0.0f : 1.0f
        );
    }

    const std::vector<GLuint> indices{
        0, 1, 3,
        0, 2, 3,
        4, 5, 7,
        4, 6 ,7,
        2, 3, 7,
        2, 6, 7,
        0, 1, 5,
        0, 4, 5,
        0, 2, 6,
        0, 4, 6,
        1, 3, 7,
        1, 5, 7
    };

    return MeshData{
        MakeIndexedVao(vertices, indices),
        static_cast<GLsizei>(indices.size())
    };
}

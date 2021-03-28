#include "construction.h"

#include <array>
#include <vector>
#include <tuple>

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

    static const size_t FLOATS_PER_VERTEX = 11;

public: // Attributes

    glm::vec3 Position;
    glm::vec3 TintRgb;
    glm::vec2 TextureUv;
    glm::vec3 Normal;

private: // Constants

    static const glm::vec3 NO_TINT_RGB;

public: // Construction

    Vertex():
        Position (0.0f),
        TintRgb  (NO_TINT_RGB),
        TextureUv(0.0f),
        Normal   (0.0f)
    {
        // Empty
    }

    std::array<float, FLOATS_PER_VERTEX> AsVertexData() const
    {
        return std::array{
            Position.x, Position.y, Position.z,
            TintRgb.x, TintRgb.y, TintRgb.z,
            TextureUv.x, TextureUv.y,
            Normal.x, Normal.y, Normal.z
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

static void SetupGlVertexLayout()
{
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::FLOATS_PER_VERTEX*sizeof(float), (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::FLOATS_PER_VERTEX*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Vertex::FLOATS_PER_VERTEX*sizeof(float), (void*)(6*sizeof(float)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, Vertex::FLOATS_PER_VERTEX*sizeof(float), (void*)(8*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

static MeshData MakeIndexedMeshData(const std::vector<Vertex> & vertices, const std::vector<GLuint> & indices)
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

    SetupGlVertexLayout();

    glBindVertexArray(oldVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, oldArrayBuffer);

    return MeshData{
        std::move(vertexArrayObject),
        static_cast<GLsizei>(indices.size()),
        true
    };
}

static MeshData MakeUnindexedMeshData(const std::vector<Vertex> & vertices)
{
    const GLuint oldVertexArray = GetBoundVertexArray();
    const GLuint oldArrayBuffer = GetBoundArrayBuffer();

    const std::vector<float> vertexData = VerticesToVertexData(vertices);

    UniqueVao          vertexArrayObject   = UniqueVao::Create();
    const UniqueBuffer vertexBufferObject  = UniqueBuffer::Create();

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, SizeOfCollectionData(vertexData), vertices.data(), GL_STATIC_DRAW);

    SetupGlVertexLayout();

    glBindVertexArray(oldVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, oldArrayBuffer);

    return MeshData{
        std::move(vertexArrayObject),
        static_cast<GLsizei>(vertexData.size()),
        false
    };
}

static std::tuple<std::vector<Vertex>, std::vector<GLuint>> CreateRawSmoothAabbMeshData(
    const glm::vec3 & minCoords,
    const glm::vec3 & maxCoords,
    const bool        mustUseAxisTint
)
{
    static const size_t VERTICES_COUNT = 8;

    const glm::vec3 positiveVertexOffset = 0.5f*(maxCoords - minCoords);
    assert(positiveVertexOffset.x > 0.0f && positiveVertexOffset.y > 0.0f && positiveVertexOffset.z > 0.0f);

    const glm::vec3 positiveVertexNormal = glm::normalize(positiveVertexOffset);

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

        // This texture UV assignment is only valid for some of the faces,
        // since due to indexed rendering the same vertex will be reused
        // in situations where UV coordinates should ideally be different.
        vertex.TextureUv = glm::vec2(
            useMinX ? 0.0f : 1.0f,
            useMinY ? 0.0f : 1.0f
        );

        // Due to vertex reuse in indexed rendering, this logic uses
        // normals for smooth shading, which are the same for every
        // vertex at matching position and directed the same way as position
        // offset from AABB center.
        vertex.Normal = glm::vec3(
            (useMinX ? -1.0f : 1.0f)*positiveVertexNormal.x,
            (useMinY ? -1.0f : 1.0f)*positiveVertexNormal.y,
            (useMinZ ? -1.0f : 1.0f)*positiveVertexNormal.z
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

    return std::make_tuple(vertices, indices);
}

static MeshData CreateIndexedAabbMeshData(const glm::vec3 & minCoords, const glm::vec3 & maxCoords, const bool mustUseAxisTint)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    std::tie(vertices, indices) = CreateRawSmoothAabbMeshData(minCoords, maxCoords, mustUseAxisTint);

    return MakeIndexedMeshData(vertices, indices);
}

static MeshData CreateUnindexedAabbMeshData(
    const glm::vec3 & minCoords,
    const glm::vec3 & maxCoords,
    const bool        mustUseAxisTint,
    const bool        mustUseSmoothShading
)
{
    static constexpr size_t UNINDEXED_VERTEX_COUNT = 6 * 2 * 3;

    std::vector<Vertex> rawVertices;
    std::vector<GLuint> indices;

    std::tie(rawVertices, indices) = CreateRawSmoothAabbMeshData(minCoords, maxCoords, mustUseAxisTint);
    assert(indices.size() == UNINDEXED_VERTEX_COUNT);

    std::vector<Vertex> vertices;
    vertices.reserve(UNINDEXED_VERTEX_COUNT);

    for (const GLuint index : indices)
    {
        assert(index < rawVertices.size());

        vertices.push_back(rawVertices[index]);
    }

    static const glm::vec2 UV_BOTTOM_LEFT (0.0f, 0.0f);
    static const glm::vec2 UV_BOTTOM_RIGHT(1.0f, 0.0f);
    static const glm::vec2 UV_TOP_LEFT    (0.0f, 1.0f);
    static const glm::vec2 UV_TOP_RIGHT   (1.0f, 1.0f);

    // Triangle 0, 1, 3
    vertices[0].TextureUv = UV_BOTTOM_RIGHT;
    vertices[1].TextureUv = UV_BOTTOM_LEFT;
    vertices[2].TextureUv = UV_TOP_LEFT;

    // Triangle 0, 2, 3
    vertices[3].TextureUv = UV_BOTTOM_RIGHT;
    vertices[4].TextureUv = UV_TOP_RIGHT;
    vertices[5].TextureUv = UV_TOP_LEFT;

    // Triangle 4, 5, 7
    vertices[6].TextureUv = UV_BOTTOM_LEFT;
    vertices[7].TextureUv = UV_BOTTOM_RIGHT;
    vertices[8].TextureUv = UV_TOP_RIGHT;

    // Triangle 4, 6 ,7
    vertices[9].TextureUv  = UV_BOTTOM_LEFT;
    vertices[10].TextureUv = UV_TOP_LEFT;
    vertices[11].TextureUv = UV_TOP_RIGHT;

    // Triangle 2, 3, 7
    vertices[12].TextureUv = UV_TOP_LEFT;
    vertices[13].TextureUv = UV_TOP_RIGHT;
    vertices[14].TextureUv = UV_BOTTOM_RIGHT;

    // Triangle 2, 6, 7
    vertices[15].TextureUv = UV_TOP_LEFT;
    vertices[16].TextureUv = UV_BOTTOM_LEFT;
    vertices[17].TextureUv = UV_BOTTOM_RIGHT;

    // Triangle 0, 1, 5
    vertices[18].TextureUv = UV_BOTTOM_LEFT;
    vertices[19].TextureUv = UV_BOTTOM_RIGHT;
    vertices[20].TextureUv = UV_TOP_RIGHT;

    // Triangle 0, 4, 5
    vertices[21].TextureUv = UV_BOTTOM_LEFT;
    vertices[22].TextureUv = UV_TOP_LEFT;
    vertices[23].TextureUv = UV_TOP_RIGHT;

    // Triangle 0, 2, 6
    vertices[24].TextureUv = UV_BOTTOM_LEFT;
    vertices[25].TextureUv = UV_TOP_LEFT;
    vertices[26].TextureUv = UV_TOP_RIGHT;

    // Triangle 0, 4, 6
    vertices[27].TextureUv = UV_BOTTOM_LEFT;
    vertices[28].TextureUv = UV_BOTTOM_RIGHT;
    vertices[29].TextureUv = UV_TOP_RIGHT;

    // Triangle 1, 3, 7
    vertices[30].TextureUv = UV_BOTTOM_RIGHT;
    vertices[31].TextureUv = UV_TOP_RIGHT;
    vertices[32].TextureUv = UV_TOP_LEFT;

    // Triangle 1, 5, 7
    vertices[33].TextureUv = UV_BOTTOM_RIGHT;
    vertices[34].TextureUv = UV_BOTTOM_LEFT;
    vertices[35].TextureUv = UV_TOP_LEFT;

    static_assert(UNINDEXED_VERTEX_COUNT - 1 == 35);

    if (!mustUseSmoothShading)
    {
        static const std::vector<std::pair<std::pair<size_t, size_t>, glm::vec3>> SHARP_NORMAL_VERTEX_RANGES{
            {{0,  6},  glm::vec3(0.0f, 0.0f, -1.0f)},
            {{6,  12}, glm::vec3(0.0f, 0.0f, 1.0f)},
            {{12, 18}, glm::vec3(0.0f, 1.0f, 0.0f)},
            {{18, 24}, glm::vec3(0.0f, -1.0f, 0.0f)},
            {{24, 30}, glm::vec3(-1.0f, 0.0f, 0.0f)},
            {{30, 36}, glm::vec3(1.0f, 0.0f, 0.0f)}
        };

        for (const auto & sharpNormalData : SHARP_NORMAL_VERTEX_RANGES)
        {
            const auto &      vertexRange = sharpNormalData.first;
            const glm::vec3 & sharpNormal = sharpNormalData.second;

            for (size_t vertexIdx = vertexRange.first; vertexIdx < vertexRange.second; vertexIdx++)
                vertices[vertexIdx].Normal = sharpNormal;
        }
    }

    return MakeUnindexedMeshData(vertices);
}

//
// Utilities
//

Mesh CreateAabbMesh(
    const bool        mustUseIndices,
    const glm::vec3 & minCoords,
    const glm::vec3 & maxCoords,
    const bool        mustUseAxisTint,
    const bool        mustUseSmoothShading
)
{
    assert(
        (mustUseSmoothShading || !mustUseIndices)
            && "indexed AABB mesh, as currently implemented, will always use smooth shading"
    );

    return Mesh(
        mustUseIndices
            ? CreateIndexedAabbMeshData  (minCoords, maxCoords, mustUseAxisTint)
            : CreateUnindexedAabbMeshData(minCoords, maxCoords, mustUseAxisTint, mustUseSmoothShading)
    );
}

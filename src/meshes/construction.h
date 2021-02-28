#pragma once

#include <glm/glm.hpp>

#include "gl/wrappers.h"

//
// Interface types
//

struct MeshData final
{
    UniqueVao VertexArrayObject;
    GLsizei   ElementsCount;
};

//
// Utilities
//

MeshData CreateAabbMesh(const glm::vec3 & minCoords, const glm::vec3 & maxCoords, const bool mustUseAxisTint);

inline MeshData CreateUnitCubeMesh(const bool isOriginCentered, const bool mustUseAxisTint)
{
    static const glm::vec3 CENTERED_ORIGIN_OFFSET(-0.5f, -0.5f, -0.5f);

    const glm::vec3 actualOffset = isOriginCentered ? CENTERED_ORIGIN_OFFSET : glm::vec3(0.0f);

    return CreateAabbMesh(
        glm::vec3(0.0f) + actualOffset,
        glm::vec3(1.0f, 1.0f, 1.0f) + actualOffset,
        mustUseAxisTint
    );
}

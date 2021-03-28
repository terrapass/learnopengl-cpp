#pragma once

#include <glm/glm.hpp>

#include "Mesh.h"

//
// Utilities
//

Mesh CreateAabbMesh(
    const bool        mustUseIndices,
    const glm::vec3 & minCoords,
    const glm::vec3 & maxCoords,
    const bool        mustUseAxisTint,
    const bool        mustUseSmoothShading
);

inline Mesh CreateUnitCubeMesh(
    const bool mustUseIndices,
    const bool isOriginCentered,
    const bool mustUseAxisTint,
    const bool mustUseSmoothShading
)
{
    static const glm::vec3 CENTERED_ORIGIN_OFFSET(-0.5f, -0.5f, -0.5f);

    const glm::vec3 actualOffset = isOriginCentered ? CENTERED_ORIGIN_OFFSET : glm::vec3(0.0f);

    return CreateAabbMesh(
        mustUseIndices,
        glm::vec3(0.0f) + actualOffset,
        glm::vec3(1.0f, 1.0f, 1.0f) + actualOffset,
        mustUseAxisTint,
        mustUseSmoothShading
    );
}

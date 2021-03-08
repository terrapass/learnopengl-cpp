#include "projections.h"

#include <glm/gtc/matrix_transform.hpp>

namespace
{

struct ProjectionMatrixCreationVisitor final
{
public: // Interface

    glm::mat4 operator()(const OrthographicProjection & orthographicProjection) const
    {
        const float halfWidth  = 0.5f*orthographicProjection.Width;
        const float halfHeight = 0.5f*orthographicProjection.Height;

        return glm::ortho(
            -halfWidth,
            halfWidth,
            -halfHeight,
            halfHeight,
            orthographicProjection.NearPlane,
            orthographicProjection.FarPlane
        );
    }

    glm::mat4 operator()(const PerspectiveProjection & perspectiveProjection) const
    {
        return glm::perspective(
            perspectiveProjection.VerticalFov,
            perspectiveProjection.AspectRatio,
            perspectiveProjection.NearPlane,
            perspectiveProjection.FarPlane
        );
    }
};

} // anonymous namespace

//
// Utilities
//

glm::mat4 CreateMatrixFromProjection(const Projection & projection)
{
    static const ProjectionMatrixCreationVisitor PROJECTION_MATRIX_CREATION_VISITOR;

    return std::visit(PROJECTION_MATRIX_CREATION_VISITOR, projection);
}

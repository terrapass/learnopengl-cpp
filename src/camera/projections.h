#pragma once

#include <variant>

#include <glm/glm.hpp>

//
// Interface types
//

struct OrthographicProjection final
{
    float Width;
    float Height;
    float NearPlane;
    float FarPlane;
};

struct PerspectiveProjection final
{
    float VerticalFov;
    float AspectRatio;
    float NearPlane;
    float FarPlane;
};

using Projection = std::variant<OrthographicProjection, PerspectiveProjection>;

//
// Utilities
//

glm::mat4 CreateMatrixFromProjection(const Projection & projection);

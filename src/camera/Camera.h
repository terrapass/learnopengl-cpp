#pragma once

#include <optional>

#include <glm/glm.hpp>

#include "projections.h"

//
// Interface types
//

struct LookAtSettings final
{
    glm::vec3 EyePosition;
    glm::vec3 Target;
    glm::vec3 EyeUpWorld;
};

//
// Utilities
//

glm::mat4 CreateMatrixFromLookAtSettings(const LookAtSettings & lookAtSettings);

//
// Camera
//

class Camera final
{
public: // Construction

    Camera(const LookAtSettings & lookAtSettings, const Projection & projection);

    Camera(const Camera &) = default;

    Camera & operator=(const Camera &) = default;

public: // Interface

    const LookAtSettings & GetLookAtSettings() const;

    LookAtSettings & GetLookAtSettings();

    const Projection & GetProjection() const;

    Projection & GetProjection();

    const glm::mat4 & GetLookAtMatrix() const;

    const glm::mat4 & GetProjectionMatrix() const;

private: // Members

    LookAtSettings m_LookAtSettings;
    Projection     m_Projection;

    mutable std::optional<glm::mat4> m_LookAtMatrix;
    mutable std::optional<glm::mat4> m_ProjectionMatrix;
};

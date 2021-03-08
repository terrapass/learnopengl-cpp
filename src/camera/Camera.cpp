#include "Camera.h"

#include <cassert>

#include <glm/ext/matrix_transform.hpp>

//
// Utilities
//

glm::mat4 CreateMatrixFromLookAtSettings(const LookAtSettings & lookAtSettings)
{
    return glm::lookAt(
        lookAtSettings.EyePosition,
        lookAtSettings.Target,
        lookAtSettings.EyeUpWorld
    );
}

//
// Construction
//

Camera::Camera(const LookAtSettings & lookAtSettings, const Projection & projection):
    m_LookAtSettings  (lookAtSettings),
    m_Projection      (projection),
    m_LookAtMatrix    (),
    m_ProjectionMatrix()
{
    // Empty
}

//
// Interface
//

const LookAtSettings & Camera::GetLookAtSettings() const
{
    return m_LookAtSettings;
}

LookAtSettings & Camera::GetLookAtSettings()
{
    m_LookAtMatrix.reset();

    return m_LookAtSettings;
}

const Projection & Camera::GetProjection() const
{
    return m_Projection;
}

Projection & Camera::GetProjection()
{
    m_ProjectionMatrix.reset();

    return m_Projection;
}

const glm::mat4 & Camera::GetLookAtMatrix() const
{
    if (!m_LookAtMatrix.has_value())
        m_LookAtMatrix.emplace(CreateMatrixFromLookAtSettings(m_LookAtSettings));

    assert(m_LookAtMatrix.has_value());
    return *m_LookAtMatrix;
}

const glm::mat4 & Camera::GetProjectionMatrix() const
{
    if (!m_ProjectionMatrix.has_value())
        m_ProjectionMatrix.emplace(CreateMatrixFromProjection(m_Projection));

    assert(m_ProjectionMatrix.has_value());
    return *m_ProjectionMatrix;
}

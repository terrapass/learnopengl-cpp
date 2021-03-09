#pragma once

#include <cassert>

#include <glm/glm.hpp>

#include "input.h"

#include "Camera.h"

//
// Service
//

namespace detail
{

class BaseCameraController
{
protected: // Construction

    inline BaseCameraController(Camera * const camera, IInputReceiver * const inputReceiver);

public: // Copy / Move

    BaseCameraController(const BaseCameraController &) = delete;

    BaseCameraController(BaseCameraController &&) = default;

    BaseCameraController & operator=(const BaseCameraController &) = delete;

    BaseCameraController & operator=(BaseCameraController &&) = default;

protected: // Members

    Camera * const         m_Camera;
    IInputReceiver * const m_InputReceiver;
};

inline BaseCameraController::BaseCameraController(Camera * const camera, IInputReceiver * const inputReceiver):
    m_Camera       (camera),
    m_InputReceiver(inputReceiver)
{
    assert(m_Camera != nullptr);
    assert(m_InputReceiver != nullptr);
}

} // namespace detail

//
// AutoRotatingCameraController
//

class AutoRotatingCameraController final:
    protected detail::BaseCameraController
{
public: // Interface types

    struct Settings final
    {
        glm::vec3 Target;
        float     RadiusBase;
        float     RadiusMaxDelta;
        float     RadiusChangeFactor;
        float     AngularSpeed;
    };

public: // Construction

    AutoRotatingCameraController(Camera * const camera, IInputReceiver * const inputReceiver, Settings settings);

public: // Interface

    void Update(const float deltaTimeSeconds);

private: // Service

    void ProcessInput(const float deltaTimeSeconds);

    void UpdateCameraLookAt();

private: // Members

    Settings m_Settings;
    float    m_CurrentRadius;
    float    m_CurrentAngle;
};

#pragma once

#include <cassert>
#include <optional>

#include <boost/signals2.hpp>

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

//
// FlyCameraController
//

class FlyCameraController final:
  protected detail::BaseCameraController
{
public: // Interface types

    struct Settings final
    {
        float MovementSpeed;
        float RotationSensitivity;
        bool  MustInvertPitch;
        // TODO: Maybe extract zoom management into a separate controller, which would work alongside this one?
        float MinVerticalFov;
        float MaxVerticalFov;
        float ZoomSensitivity;
    };

public: // Construction

    FlyCameraController(Camera * const camera, IInputReceiver * const inputReceiver, Settings settings);

public: // Copy / Move

    FlyCameraController(const FlyCameraController &) = delete;

    FlyCameraController(FlyCameraController &&);

    FlyCameraController & operator=(const FlyCameraController &) = delete;

    FlyCameraController & operator=(FlyCameraController &&);

public: // Interface

    void Update(const float deltaTimeSeconds);

    bool IsEnabled() const;

    void SetEnabled(const bool value);

private: // Service

    void ProcessInput(LookAtSettings & cameraLookAtSettings, const float deltaTimeSeconds);

    void UpdateLookDirection();

    static float ExtractYawFromLookDirection(const glm::vec3 & lookDirection);

    static float ExtractPitchFromLookDirection(const glm::vec3 & lookDirection);

private: // Events

    void OnMouseMoved(const MouseState & mouseState);

    void OnScroll(const glm::vec2 scrollOffset);

private: // Members

    Settings  m_Settings;

    boost::signals2::scoped_connection m_MouseMovedSignalConnection;
    boost::signals2::scoped_connection m_ScrollSignalConnection;

    std::optional<boost::signals2::shared_connection_block> m_MouseMovedSignalConnectionBlock;
    std::optional<boost::signals2::shared_connection_block> m_ScrollSignalConnectionBlock;

    glm::vec3 m_LookDirection;
    float     m_Yaw;
    float     m_Pitch;

    // TODO: If needed, move up the hierarchy to BaseCameraController, along with corresponding interface methods.
    bool m_IsEnabled;
};

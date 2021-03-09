#include "controllers.h"

#include <cmath>
#include <numbers>

//
// AutoRotatingCameraController
//

//
// Construction
//

AutoRotatingCameraController::AutoRotatingCameraController(
    Camera * const         camera,
    IInputReceiver * const inputReceiver,
    Settings               settings
):
    BaseCameraController(camera, inputReceiver),
    m_Settings          (std::move(settings)),
    m_CurrentRadius     (m_Settings.RadiusBase),
    m_CurrentAngle      (0.0f)
{
    UpdateCameraLookAt();
}

//
// Interface
//

void AutoRotatingCameraController::Update(const float deltaTimeSeconds)
{
    m_CurrentAngle += deltaTimeSeconds*m_Settings.AngularSpeed;
    m_CurrentAngle = std::fmod(m_CurrentAngle, 2.0f*std::numbers::pi_v<float>);

    ProcessInput(deltaTimeSeconds);

    UpdateCameraLookAt();
}

//
// Service
//

void AutoRotatingCameraController::ProcessInput(const float deltaTimeSeconds)
{
    const float minRadius = m_Settings.RadiusBase - m_Settings.RadiusMaxDelta;
    const float maxRadius = m_Settings.RadiusBase + m_Settings.RadiusMaxDelta;

    const float deltaRadius = deltaTimeSeconds*m_Settings.RadiusChangeFactor;

    if (m_CurrentRadius < maxRadius && m_InputReceiver->IsKeyDown(Key::Down))
        m_CurrentRadius += deltaRadius;
    else if (m_CurrentRadius > minRadius && m_InputReceiver->IsKeyDown(Key::Up))
        m_CurrentRadius -= deltaRadius;
}

void AutoRotatingCameraController::UpdateCameraLookAt()
{
    static const LookAtSettings CAMERA_LOOK_AT_SETTINGS_PROTOTYPE{
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    };

    LookAtSettings cameraLookAtSettings(CAMERA_LOOK_AT_SETTINGS_PROTOTYPE);
    cameraLookAtSettings.EyePosition.x = m_CurrentRadius*glm::sin(m_CurrentAngle);
    cameraLookAtSettings.EyePosition.z = m_CurrentRadius*glm::cos(m_CurrentAngle);

    m_Camera->GetLookAtSettings() = std::move(cameraLookAtSettings);
}

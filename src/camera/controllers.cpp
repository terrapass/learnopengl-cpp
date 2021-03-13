#include "controllers.h"

#include <cmath>
#include <numbers>
#include <algorithm>
#include <cmath>

#include "logging.h"

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

//
// FlyCameraController
//

FlyCameraController::FlyCameraController(
    Camera * const         camera,
    IInputReceiver * const inputReceiver,
    Settings               settings
):
    BaseCameraController             (camera, inputReceiver),
    m_Settings                       (std::move(settings)),
    m_MouseMovedSignalConnection     (),
    m_ScrollSignalConnection         (),
    m_MouseMovedSignalConnectionBlock(std::nullopt),
    m_ScrollSignalConnectionBlock    (std::nullopt),
    m_LookDirection                  (m_Camera->GetLookAtSettings().GetLookDirectionNormalized()),
    m_Yaw                            (ExtractYawFromLookDirection(m_LookDirection)),
    m_Pitch                          (ExtractPitchFromLookDirection(m_LookDirection)),
    m_IsEnabled                      (true)
{
    assert(!m_MouseMovedSignalConnection.connected());
    assert(!m_ScrollSignalConnection.connected());

    m_MouseMovedSignalConnection = m_InputReceiver->MouseMovedSignal.connect(
        std::bind(&FlyCameraController::OnMouseMoved, this, std::placeholders::_1)
    );
    m_ScrollSignalConnection = m_InputReceiver->ScrollSignal.connect(
        std::bind(&FlyCameraController::OnScroll, this, std::placeholders::_1)
    );
}

//
// Interface
//

void FlyCameraController::Update(const float deltaTimeSeconds)
{
    if (!IsEnabled())
        return;

    UpdateLookDirection();

    LookAtSettings & cameraLookAtSettings = m_Camera->GetLookAtSettings();

    ProcessInput(cameraLookAtSettings, deltaTimeSeconds);

    cameraLookAtSettings.SetLookDirectionRaw(m_LookDirection);
}

bool FlyCameraController::IsEnabled() const
{
    return m_IsEnabled;
}

void FlyCameraController::SetEnabled(const bool value)
{
    if (m_IsEnabled == value)
        return;

    m_IsEnabled = value;

    if (m_IsEnabled)
    {
        m_MouseMovedSignalConnectionBlock.reset();
        m_ScrollSignalConnectionBlock.reset();
    }
    else
    {
        m_MouseMovedSignalConnectionBlock.emplace(m_MouseMovedSignalConnection);
        m_ScrollSignalConnectionBlock.emplace(m_ScrollSignalConnection);
    }
}

//
// Service
//

void FlyCameraController::ProcessInput(LookAtSettings & cameraLookAtSettings, const float deltaTimeSeconds)
{
    const glm::vec3 lookDirectionRight = glm::normalize(
        glm::cross(m_LookDirection, cameraLookAtSettings.EyeUpWorld)
    );

    if (m_InputReceiver->IsKeyDown(Key::W))
        cameraLookAtSettings.EyePosition += deltaTimeSeconds*m_LookDirection;

    if (m_InputReceiver->IsKeyDown(Key::S))
        cameraLookAtSettings.EyePosition -= deltaTimeSeconds*m_LookDirection;

    if (m_InputReceiver->IsKeyDown(Key::A))
        cameraLookAtSettings.EyePosition -= deltaTimeSeconds*lookDirectionRight;

    if (m_InputReceiver->IsKeyDown(Key::D))
        cameraLookAtSettings.EyePosition += deltaTimeSeconds*lookDirectionRight;
}

void FlyCameraController::UpdateLookDirection()
{
    m_LookDirection = glm::normalize(glm::vec3(
        cos(m_Yaw)*cos(m_Pitch),
        sin(m_Pitch),
        sin(m_Yaw)*cos(m_Pitch)
    ));
}

float FlyCameraController::ExtractYawFromLookDirection(const glm::vec3 & lookDirection)
{
    return atan2(lookDirection.z, lookDirection.x);
}

float FlyCameraController::ExtractPitchFromLookDirection(const glm::vec3 & lookDirection)
{
    return asin(lookDirection.y);
}


//
// Events
//

void FlyCameraController::OnMouseMoved(const MouseState & mouseState)
{
    constexpr float DOUBLE_PI      = 2.0f*std::numbers::pi_v<float>;
    constexpr float ALMOST_HALF_PI = 0.499f*std::numbers::pi_v<float>;

    if (!mouseState.CursorPositionDelta.has_value())
        return;

    glm::vec2 eulerAnglesDelta = m_Settings.RotationSensitivity*mouseState.CursorPositionDelta.value();

    m_Yaw = std::fmod(m_Yaw + eulerAnglesDelta.x, DOUBLE_PI);

    const float pitchInversionMultiplier = m_Settings.MustInvertPitch ? 1.0f : -1.0f;

    // Clamp pitch strictly between -90 and 90 degrees to avoid look-at flip
    m_Pitch = std::clamp(m_Pitch + pitchInversionMultiplier*eulerAnglesDelta.y, -ALMOST_HALF_PI, ALMOST_HALF_PI);
}

void FlyCameraController::OnScroll(const glm::vec2 scrollOffset)
{
    Projection & cameraProjection = m_Camera->GetProjection();

    if (!std::holds_alternative<PerspectiveProjection>(cameraProjection))
    {
        BOOST_LOG_TRIVIAL(warning)<< "Ignoring scroll since the controlled camera does NOT have perspective projection";

        return;
    }

    PerspectiveProjection & cameraPerspectiveProjection = std::get<PerspectiveProjection>(cameraProjection);

    cameraPerspectiveProjection.VerticalFov = std::clamp(
        cameraPerspectiveProjection.VerticalFov - m_Settings.ZoomSensitivity*scrollOffset.y,
        m_Settings.MinVerticalFov,
        m_Settings.MaxVerticalFov
    );
}

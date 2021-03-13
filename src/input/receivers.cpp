#include "receivers.h"

#include <cassert>

#include "utils/glfw_utils.h"
#include "logging.h"

//
// Statics
//

std::unique_ptr<GlfwInputReceiver> GlfwInputReceiver::s_Instance;

//
// Singleton accessor
//

GlfwInputReceiver * GlfwInputReceiver::GetInstance()
{
    assert(
        s_Instance != nullptr
            && "GlfwInputReceiver::InitializeInstance() must be called before the first call to GlfwInputReceiver::GetInstance()"
    );
    return s_Instance.get();
}

//
// Construction
//

GlfwInputReceiver::GlfwInputReceiver(GLFWwindow * const window):
    m_Window(window)
{
    assert(m_Window != nullptr);
}

//
// IInputReceiver
//

bool GlfwInputReceiver::IsKeyDown(const Key key) const
{
    return glfwGetKey(m_Window, KeyToGlfw(key)) == GLFW_PRESS;
}

bool GlfwInputReceiver::IsMouseButtonDown(const MouseButton mouseButton) const
{
    return m_MouseState.IsMouseButtonPressed(mouseButton);
}

//
// Interface
//

void GlfwInputReceiver::InitializeInstance(GLFWwindow * const window)
{
    assert(s_Instance == nullptr && "GlfwInputReceiver::InitializeInstance() must be called only once");

    glfwSetKeyCallback(window, &GlfwInputReceiver::OnGlfwKeyEvent);
    glfwSetCursorPosCallback(window, &GlfwInputReceiver::OnGlfwCursorPositionChanged);
    glfwSetMouseButtonCallback(window, &GlfwInputReceiver::OnGlfwMouseButtonEvent);

    s_Instance = std::unique_ptr<GlfwInputReceiver>(new GlfwInputReceiver(window));
}

//
// Service
//

void GlfwInputReceiver::OnKeyPressed(const Key key)
{
    KeyPressedSignal(key);
}

void GlfwInputReceiver::OnCursorPositionChanged(const float cursorX, const float cursorY)
{
    const glm::vec2 previousPosition = m_MouseState.CursorPosition;

    m_MouseState.CursorPosition      = glm::vec2(cursorX, cursorY);
    m_MouseState.CursorPositionDelta = m_MouseState.CursorPosition - previousPosition;

    MouseMovedSignal(m_MouseState);
}

void GlfwInputReceiver::OnMouseButtonPressed(const MouseButton mouseButton)
{
    m_MouseState.SetMouseButtonPressed(mouseButton, true);

    MouseButtonPressedSignal(mouseButton, m_MouseState);
}

void GlfwInputReceiver::OnMouseButtonReleased(const MouseButton mouseButton)
{
    m_MouseState.SetMouseButtonPressed(mouseButton, false);

    MouseButtonReleasedSignal(mouseButton, m_MouseState);
}

void GlfwInputReceiver::OnGlfwKeyEvent(
    GLFWwindow * const window,
    const int          key,
    const int          /*scancode*/,
    const int          action,
    const int          /*mods*/
)
{
    if (window != GlfwInputReceiver::GetInstance()->m_Window)
    {
        BOOST_LOG_TRIVIAL(warning)<< "Ignoring key event because the source window is different from the one GlfwInputReceiver has been initialized with";

        return;
    }

    if (action != GLFW_PRESS)
        return;

    GlfwInputReceiver::GetInstance()->OnKeyPressed(KeyFromGlfw(key));
}

void GlfwInputReceiver::OnGlfwCursorPositionChanged(
    GLFWwindow * const window,
    const double       cursorX,
    const double       cursorY
)
{
    if (window != GlfwInputReceiver::GetInstance()->m_Window)
    {
        BOOST_LOG_TRIVIAL(debug)<< "Ignoring cursor position change because the source window is different from the one GlfwInputReceiver has been initialized with";

        return;
    }

    GlfwInputReceiver::GetInstance()->OnCursorPositionChanged(static_cast<float>(cursorX), static_cast<float>(cursorY));
}

void GlfwInputReceiver::OnGlfwMouseButtonEvent(
    GLFWwindow * const window,
    const int          button,
    const int          action,
    const int          /*mods*/
)
{
    if (window != GlfwInputReceiver::GetInstance()->m_Window)
    {
        BOOST_LOG_TRIVIAL(warning)<< "Ignoring mouse button event because the source window is different from the one GlfwInputReceiver has been initialized with";

        return;
    }

    if (action == GLFW_REPEAT)
        return;

    const MouseButton mouseButton = MouseButtonFromGlfw(button);

    if (action == GLFW_PRESS)
    {
        GlfwInputReceiver::GetInstance()->OnMouseButtonPressed(mouseButton);
    }
    else
    {
        assert(action == GLFW_RELEASE);

        GlfwInputReceiver::GetInstance()->OnMouseButtonReleased(mouseButton);
    }

}

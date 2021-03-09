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

//
// Interface
//

void GlfwInputReceiver::InitializeInstance(GLFWwindow * const window)
{
    assert(s_Instance == nullptr && "GlfwInputReceiver::InitializeInstance() must be called only once");

    glfwSetKeyCallback(window, &GlfwInputReceiver::OnGlfwKeyEvent);

    s_Instance = std::unique_ptr<GlfwInputReceiver>(new GlfwInputReceiver(window));
}

//
// Service
//

void GlfwInputReceiver::OnGlfwKeyEvent(
    GLFWwindow * const window,
    const int          key,
    const int          /*scancode*/,
    const int          action,
    const int          /*mods*/
)
{
    if (window != GlfwInputReceiver::GetInstance()->m_Window)
        BOOST_LOG_TRIVIAL(warning)<< "Ignoring key event because the source window is different from the one GlfwInputReceiver has been initialized with";

    if (action != GLFW_PRESS)
        return;

    GlfwInputReceiver::GetInstance()->KeyPressedSignal(KeyFromGlfw(key));
}

#include "mouse_button_mapping.h"

#include <unordered_map>

#include <GLFW/glfw3.h>

//
// Constants
//

static const std::unordered_map<int, MouseButton> MOUSE_BUTTON_FROM_GLFW{
    {GLFW_MOUSE_BUTTON_LEFT,   MouseButton::Left},
    {GLFW_MOUSE_BUTTON_RIGHT,  MouseButton::Right},
    {GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::Middle}
};

//
// Utilities
//

MouseButton MouseButtonFromGlfw(const int glfwMouseButton)
{
    const auto mouseButtonIt = MOUSE_BUTTON_FROM_GLFW.find(glfwMouseButton);

    return mouseButtonIt != MOUSE_BUTTON_FROM_GLFW.cend()
        ? mouseButtonIt->second
        : MouseButton::Unknown;
}

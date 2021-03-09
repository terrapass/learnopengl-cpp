#include "key_mapping.h"

#include <unordered_map>

#include <GLFW/glfw3.h>

//
// Constants
//

static const std::unordered_map<int, Key> KEY_FROM_GLFW{
    {GLFW_KEY_ESCAPE, Key::Escape},
    {GLFW_KEY_Z,      Key::Z},
    {GLFW_KEY_W,      Key::W},
    {GLFW_KEY_S,      Key::S},
    {GLFW_KEY_A,      Key::A},
    {GLFW_KEY_D,      Key::D},
    {GLFW_KEY_UP,     Key::Up},
    {GLFW_KEY_DOWN,   Key::Down},
    {GLFW_KEY_LEFT,   Key::Left},
    {GLFW_KEY_RIGHT,  Key::Right}
};

//
// Utilities
//

Key KeyFromGlfw(const int glfwKey)
{
    const auto KeyIt = KEY_FROM_GLFW.find(glfwKey);

    return KeyIt != KEY_FROM_GLFW.cend()
        ? KeyIt->second
        : Key::Unknown;
}

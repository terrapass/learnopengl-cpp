#include "key_mapping.h"

#include <unordered_map>
#include <cassert>

#include <GLFW/glfw3.h>

//
// Constants
//

static const std::unordered_map<int, Key> KEY_FROM_GLFW{
    {GLFW_KEY_UNKNOWN, Key::Unknown},
    {GLFW_KEY_ESCAPE,  Key::Escape},
    {GLFW_KEY_Z,       Key::Z},
    {GLFW_KEY_W,       Key::W},
    {GLFW_KEY_S,       Key::S},
    {GLFW_KEY_A,       Key::A},
    {GLFW_KEY_D,       Key::D},
    {GLFW_KEY_UP,      Key::Up},
    {GLFW_KEY_DOWN,    Key::Down},
    {GLFW_KEY_LEFT,    Key::Left},
    {GLFW_KEY_RIGHT,   Key::Right}
};

//
// Utilities
//

Key KeyFromGlfw(const int glfwKey)
{
    const auto keyIt = KEY_FROM_GLFW.find(glfwKey);

    return keyIt != KEY_FROM_GLFW.cend()
        ? keyIt->second
        : Key::Unknown;
}

int KeyToGlfw(const Key key)
{
    static std::unordered_map<Key, int> keyToGlfw;
    if (keyToGlfw.empty())
    {
        for (const auto & Pair : KEY_FROM_GLFW)
            keyToGlfw.emplace(Pair.second, Pair.first);
    }

    const auto glfwKeyIt = keyToGlfw.find(key);
    if (glfwKeyIt == keyToGlfw.cend())
    {
        assert(false && "every defined key must have a mapping to GLFW");

        return GLFW_KEY_UNKNOWN;
    }

    return glfwKeyIt->second;
}

#pragma once

#include <concepts>
#include <memory>

#include <boost/signals2.hpp>

#include <GLFW/glfw3.h>

#include "Key.h"

//
// Concepts
//

template <typename T>
concept InputReceiver = requires(T inputReceiver) {
    typename T::todo;
};

//
// GlfwInputReceiver
//

class GlfwInputReceiver final
{
public: // Singleton accessor

    static GlfwInputReceiver * GetInstance();

private: // Construction

    explicit GlfwInputReceiver(GLFWwindow * const window);

public: // Destruction

    ~GlfwInputReceiver() = default;

private: // Copy / Move

    GlfwInputReceiver(const GlfwInputReceiver &) = delete;

    GlfwInputReceiver & operator=(const GlfwInputReceiver &) = delete;

public: // Interface

    static void InitializeInstance(GLFWwindow * const window);

    bool IsKeyDown() const;

private: // Service

    static void OnGlfwKeyEvent(
        GLFWwindow * const window,
        const int          key,
        const int          scancode,
        const int          action,
        const int          mods
    );

public: // Signals

    boost::signals2::signal<void(Key)> KeyPressedSignal;

private: // Statics

    static std::unique_ptr<GlfwInputReceiver> s_Instance;

private: // Members

    GLFWwindow * const m_Window;
};

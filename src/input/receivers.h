#pragma once

#include <concepts>
#include <memory>

#include <boost/signals2.hpp>

#include <GLFW/glfw3.h>

#include "Key.h"

//
// IInputReceiver
//

struct IInputReceiver
{
public: // Interface

    virtual bool IsKeyDown(const Key key) const = 0;

public: // Signals

    boost::signals2::signal<void(Key)> KeyPressedSignal;
};

//
// GlfwInputReceiver
//

class GlfwInputReceiver final:
    public IInputReceiver
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

public: // IInputReceiver

    bool IsKeyDown(const Key key) const override;

public: // Interface

    static void InitializeInstance(GLFWwindow * const window);

private: // Service

    static void OnGlfwKeyEvent(
        GLFWwindow * const window,
        const int          key,
        const int          scancode,
        const int          action,
        const int          mods
    );

private: // Statics

    static std::unique_ptr<GlfwInputReceiver> s_Instance;

private: // Members

    GLFWwindow * const m_Window;
};

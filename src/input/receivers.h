#pragma once

#include <concepts>
#include <memory>

#include <boost/signals2.hpp>

#include <GLFW/glfw3.h>

#include "Key.h"
#include "MouseState.h"

//
// IInputReceiver
//

struct IInputReceiver
{
public: // Interface

    virtual bool IsKeyDown(const Key key) const = 0;

    virtual bool IsMouseButtonDown(const MouseButton mouseButton) const = 0;

public: // Signals

    boost::signals2::signal<void(Key)>                             KeyPressedSignal;
    boost::signals2::signal<void(const MouseState &)>              MouseMovedSignal;
    boost::signals2::signal<void(MouseButton, const MouseState &)> MouseButtonPressedSignal;
    boost::signals2::signal<void(MouseButton, const MouseState &)> MouseButtonReleasedSignal;
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

    bool IsMouseButtonDown(const MouseButton mouseButton) const override;

public: // Interface

    static void InitializeInstance(GLFWwindow * const window);

private: // Service

    void OnKeyPressed(const Key key);

    void OnCursorPositionChanged(const float cursorX, const float cursorY);

    void OnMouseButtonPressed(const MouseButton mouseButton);

    void OnMouseButtonReleased(const MouseButton mouseButton);

    static void OnGlfwKeyEvent(
        GLFWwindow * const window,
        const int          key,
        const int          scancode,
        const int          action,
        const int          mods
    );

    static void OnGlfwCursorPositionChanged(
        GLFWwindow * const window,
        const double       mouseX,
        const double       mouseY
    );

    static void OnGlfwMouseButtonEvent(
        GLFWwindow * const window,
        const int          button,
        const int          action,
        const int          mods
    );

private: // Statics

    static std::unique_ptr<GlfwInputReceiver> s_Instance;

private: // Members

    GLFWwindow * const m_Window;

    MouseState m_MouseState;
};

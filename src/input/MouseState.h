#pragma once

#include <optional>

#include <glm/glm.hpp>

//
// Interface types
//

enum class MouseButton: unsigned char
{
    None    = 0x0,

    Left    = (0x1 << 0),
    Right   = (0x1 << 1),
    Middle  = (0x1 << 2),
    Unknown = (0x1 << 3),

    Any     = 0xFF
};

//
// MouseState
//

struct MouseState final
{
public: // Attributes

    glm::vec2                CursorPosition;
    std::optional<glm::vec2> CursorPositionDelta;
    MouseButton              PressedMouseButtons;

public: // Construction

    MouseState():
        CursorPosition     (0.0f),
        CursorPositionDelta(std::nullopt),
        PressedMouseButtons(MouseButton::None)
    {
        // Empty
    }

public: // Interface

    inline bool IsMouseButtonPressed(const MouseButton mouseButton) const
    {
        return (static_cast<unsigned char>(mouseButton) & static_cast<unsigned char>(PressedMouseButtons)) != 0;
    }

    inline void SetMouseButtonPressed(const MouseButton mouseButton, const bool isPressed)
    {
        if (isPressed)
        {
            PressedMouseButtons = static_cast<MouseButton>(
                static_cast<unsigned char>(PressedMouseButtons) | static_cast<unsigned char>(mouseButton)
            );
        }
        else
        {
            PressedMouseButtons = static_cast<MouseButton>(
                static_cast<unsigned char>(PressedMouseButtons) & (~static_cast<unsigned char>(mouseButton))
            );
        }
    }
};

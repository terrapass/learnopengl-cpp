#pragma once

#include <stdexcept>

struct ScopedGLFW final
{
public: // Interface types

    struct GLFWInitFailed final: std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

public: // Construction / Destruction
    ScopedGLFW();

    ~ScopedGLFW();
};

#pragma once

#include <stdexcept>

struct ScopedGLFW final
{
public: // Interface types

    class GLFWInitFailedException final: public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

public: // Construction / Destruction
    ScopedGLFW();

    ~ScopedGLFW();
};

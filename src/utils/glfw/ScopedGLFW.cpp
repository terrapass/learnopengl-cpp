#include "ScopedGLFW.h"

#include <GLFW/glfw3.h>
#include <boost/log/trivial.hpp>

//
// Construction / Destruction
//

ScopedGLFW::ScopedGLFW()
{
    if (glfwInit())
    {
        BOOST_LOG_TRIVIAL(info)<< "Initialized GLFW";

        return;
    }

    const char * error = nullptr;
    glfwGetError(&error);

    throw GLFWInitFailedException(error);
}

ScopedGLFW::~ScopedGLFW()
{
    glfwTerminate();

    BOOST_LOG_TRIVIAL(info)<< "Terminated GLFW";
}

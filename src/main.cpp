#include <memory>
#include <cassert>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <boost/log/trivial.hpp>

#include "utils/ScopedGLFW.h"

//
// Constants
//

static constexpr int OPENGL_MAJOR_VERSION = 3;
static constexpr int OPENGL_MINOR_VERSION = 3;

static const char * const WINDOW_TITLE = "learnopengl-cpp";

static constexpr int WINDOW_WIDTH  = 800;
static constexpr int WINDOW_HEIGHT = 600;

static constexpr int MAIN_ERR_NONE        = 0;
static constexpr int MAIN_ERR_UNKNOWN     = -1;
static constexpr int MAIN_ERR_INIT_FAILED = -2;

//
// Forward declarations
//

static void SetGLViewportSize(const int width, const int height);

static void OnFramebufferSizeChanged(GLFWwindow * const /*window*/, const int width, const int height);

static void ProcessInput(GLFWwindow * const window);

//
// Main
//

int main()
{
    try
    {
        ScopedGLFW scopedGlfw;

        glfwSetErrorCallback(
            [] (auto errorCode, auto description)
            {
                BOOST_LOG_TRIVIAL(error)<< "GLFW error " << errorCode << ": " << description;
            }
        );

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        const std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window(
            glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr),
            &glfwDestroyWindow
        );

        if (!window)
        {
            assert(false && "GLFW window creation must succeed");
            BOOST_LOG_TRIVIAL(fatal)<< "Failed to create GLFW window";

            return MAIN_ERR_INIT_FAILED;
        }

        glfwMakeContextCurrent(window.get());

        if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
        {
            assert(false && "GLAD loading must succeed");
            BOOST_LOG_TRIVIAL(fatal)<< "Failed to load GLAD";

            return MAIN_ERR_INIT_FAILED;
        }

        assert(
            GLVersion.major == OPENGL_MAJOR_VERSION
                && GLVersion.minor == OPENGL_MINOR_VERSION
                && "GLAD GL version must match the expected one"
        );
        BOOST_LOG_TRIVIAL(info)<< "Loaded GLAD for OpenGL version " << GLVersion.major << '.' << GLVersion.minor;

        BOOST_LOG_TRIVIAL(info)<< "Using OpenGL version " << glGetString(GL_VERSION);
        BOOST_LOG_TRIVIAL(info)<< "Using OpenGL renderer " << glGetString(GL_RENDERER);

        {
            int framebufferWidth  = -1;
            int framebufferHeight = -1;

            glfwGetFramebufferSize(window.get(), &framebufferWidth, &framebufferHeight);

            OnFramebufferSizeChanged(window.get(), framebufferWidth, framebufferHeight);
        }

        glfwSetFramebufferSizeCallback(window.get(), &OnFramebufferSizeChanged);

        while (!glfwWindowShouldClose(window.get()))
        {
            ProcessInput(window.get());

            // TODO: Actual render commands
            glClearColor(0.5f, 0.75f, 0.75f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            // END TODO

            glfwSwapBuffers(window.get());
            glfwPollEvents();
        }
    }
    catch (const ScopedGLFW::GLFWInitFailed & e)
    {
        BOOST_LOG_TRIVIAL(fatal)<< "Failed to init GLFW: " << e.what();

        return MAIN_ERR_INIT_FAILED;
    }
    catch (const std::exception & e)
    {
        BOOST_LOG_TRIVIAL(fatal)<< "Unexpected error: " << e.what();

        return MAIN_ERR_UNKNOWN;
    }
    catch (...)
    {
        BOOST_LOG_TRIVIAL(fatal)<< "Unknown error";

        return MAIN_ERR_UNKNOWN;
    }

    return MAIN_ERR_NONE;
}

//
// Service
//

static void SetGLViewportSize(const int width, const int height)
{
    assert(width > 0);
    assert(height > 0);

    glViewport(0, 0, width, height);

    BOOST_LOG_TRIVIAL(debug)<< "Set GL viewport size to " << width << 'x' << height;
}

static void OnFramebufferSizeChanged(GLFWwindow * const /*window*/, const int width, const int height)
{
    BOOST_LOG_TRIVIAL(info)<< "Framebuffer size changed to " << width << 'x' << height;

    SetGLViewportSize(width, height);
}

static void ProcessInput(GLFWwindow * const window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

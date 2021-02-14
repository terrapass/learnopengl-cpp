#include <memory>
#include <cassert>
#include <vector>
#include <array>
#include <cmath>
#include <numbers>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "gl/constants.h"
#include "gl/wrappers.h"
#include "gl/utils.h"
#include "gl/shaders.h"
#include "gl/StatefulShaderProgram.h"
#include "textures/loading.h"
#include "utils/boost_utils.h"
#include "utils/glfw_utils.h"
#include "utils/collection_utils.h"
#include "config.h"
#include "logging.h"

//
// Constants
//

static constexpr int MAIN_ERR_NONE        = 0;
static constexpr int MAIN_ERR_UNKNOWN     = -1;
static constexpr int MAIN_ERR_INIT_FAILED = -2;

static const std::string SHADER_SOURCES_MATCHING_FILENAME = "basic";

//
// Forward declarations
//

#ifdef GLAD_DEBUG
static void OnGladFunctionCalled(const char * const funcName, void * const funcPtr, const int varArgsCount, ...);
#endif

static void OnFramebufferSizeChanged(GLFWwindow * const /*window*/, const int width, const int height);

static void OnKeyEvent(
    GLFWwindow * const window,
    const int          key,
    const int          scancode,
    const int          action,
    const int          mods
);

static void ProcessInput(GLFWwindow * const window);

//
// Main
//

int main()
{
    InitLogger();
    LogBoostVersion();

#ifdef GLAD_DEBUG
    BOOST_LOG_TRIVIAL(info)<< "Using GLAD with debug callbacks";
#ifdef NDEBUG
    BOOST_LOG_TRIVIAL(warning)<< "Using slower GLAD with debug callbacks in Release build";
#endif // NDEBUG

    glad_set_post_callback(OnGladFunctionCalled);
#endif // GLAD_DEBUG

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

        LogGlInfo();

        {
            int framebufferWidth  = -1;
            int framebufferHeight = -1;

            glfwGetFramebufferSize(window.get(), &framebufferWidth, &framebufferHeight);

            OnFramebufferSizeChanged(window.get(), framebufferWidth, framebufferHeight);
        }

        glfwSetFramebufferSizeCallback(window.get(), &OnFramebufferSizeChanged);

        // Set default polygon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // TODO0: Extract (scene setup logic, shader program loading) and refactor
        const std::vector<float> vertices{
            // Positions           // RGBs              // Texture UVs
            -0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
            -0.5f, 0.5f, 0.0f,     0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
            0.5f, 0.5f, 0.0f,      1.0f, 1.0f, 0.0f,    1.0f, 1.0f,
            -0.8f, -0.8f, 0.0f,    0.0f, 0.0f, 1.0f,    0.5f, 0.5f,
            -0.6f, -0.6f, 0.0f,    1.0f, 0.0f, 1.0f,    1.0f, 0.0f,
            -0.7f, 0.2f, 0.0f,     0.0f, 1.0f, 1.0f,    0.0f, 1.0f
        };

        const std::vector<GLuint> indices0{
            0, 1, 2,
            1, 2, 3
        };

        // SECTION: VAO setup
        const std::vector<UniqueVao>    vertexArrayObjects   = UniqueVao::CreateMany(2);
        const std::vector<UniqueBuffer> vertexBufferObjects  = UniqueBuffer::CreateMany(2);

        const UniqueBuffer elementBufferObject0 = UniqueBuffer::Create();

        glBindVertexArray(vertexArrayObjects[0]);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[0]);
        glBufferData(GL_ARRAY_BUFFER, 4*8*sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject0);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(float), indices0.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(vertexArrayObjects[1]);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);
        glBufferData(GL_ARRAY_BUFFER, 3*8*sizeof(float), vertices.data() + 4*8, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(INVALID_OPENGL_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, INVALID_OPENGL_BUFFER);
        // END SECTION

        // SECTION: Texture setup
        const UniqueTexture texture = UniqueTexture::Create();

        {
            const TextureData       textureData     = LoadTextureDataFromFile("rtwe_output_cropped.jpeg");
            const TextureMetadata & textureMetadata = textureData.GetMetadata();

            glBindTexture(GL_TEXTURE_2D, texture);

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGB,
                textureMetadata.Width,
                textureMetadata.Height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                textureData.GetData()
            );
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, INVALID_OPENGL_TEXTURE);
        }
        // END SECTION

        // SECTION: Shader setup
        // StatefulShaderProgram shaderProgram(MakeShaderProgramFromFilesPack(
        //     "xp_animation.vert",
        //     "basic_attrib.frag"
        // ));
        StatefulShaderProgram shaderProgram(MakeShaderProgramFromMatchingFiles("basic_texture"));

        glUseProgram(shaderProgram.Get());

        // TODO: Set constant uniforms

        glUseProgram(INVALID_OPENGL_SHADER);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // END SECTION

        // END TODO0

        glfwSetKeyCallback(window.get(), &OnKeyEvent);

        while (!glfwWindowShouldClose(window.get()))
        {
            ProcessInput(window.get());

            // TODO: Extract as scene render logic
            glClearColor(0.3f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shaderProgram.Use();

            glBindTexture(GL_TEXTURE_2D, texture);

            glBindVertexArray(vertexArrayObjects[0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(vertexArrayObjects[1]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            // END TODO

            glfwSwapBuffers(window.get());
            glfwPollEvents();
        }
    }
    catch (const ScopedGLFW::GLFWInitFailedException & e)
    {
        BOOST_LOG_TRIVIAL(fatal)<< "Failed to init GLFW: " << e.what();

        return MAIN_ERR_INIT_FAILED;
    }
    catch (const std::exception & e)
    {
        BOOST_LOG_TRIVIAL(fatal)<< "Fatal error: " << e.what();

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

#ifdef GLAD_DEBUG
static void OnGladFunctionCalled(const char * const funcName, void * const /*funcPtr*/, const int /*varArgsCount*/, ...)
{
    const GLenum errorCode = glad_glGetError();

    if (errorCode == GL_NO_ERROR)
        return;

    BOOST_LOG_TRIVIAL(error)<< "OpenGL error " << errorCode << " (" << GlErrorToCStr(errorCode) << ") in " << funcName;

    assert(false && "OpenGL calls must succeed");
}
#endif

static void OnFramebufferSizeChanged(GLFWwindow * const /*window*/, const int width, const int height)
{
    BOOST_LOG_TRIVIAL(info)<< "Framebuffer size changed to " << width << 'x' << height;

    SetViewportSize(width, height);
}

static void OnKeyEvent(
    GLFWwindow * const window,
    const int          key,
    const int          /*scancode*/,
    const int          action,
    const int          /*mods*/
)
{
    if (action != GLFW_PRESS)
        return;

    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;

    case GLFW_KEY_Z:
        TogglePolygonMode();
        break;
    }
}

static void ProcessInput(GLFWwindow * const /*window*/)
{
    // TODO: Process continuous key presses, mouse position etc.
}

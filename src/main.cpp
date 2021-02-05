#include <memory>
#include <cassert>
#include <vector>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <boost/log/trivial.hpp>

#include "gl/constants.h"
#include "gl/wrappers.h"
#include "utils/ScopedGLFW.h"
#include "utils/collection_utils.h"
#include "utils/file_utils.h"

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

static const std::string SHADERS_DIR = "assets/shaders/";

static const std::string VERTEX_SHADER_SOURCE_FILENAME   = "basic.vert";
static const std::vector<std::string> FRAGMENT_SHADER_SOURCE_FILENAMES{"basic0.frag", "basic1.frag"};

//
// Forward declarations
//

#ifdef GLAD_DEBUG
static void OnGladFunctionCalled(const char * const funcName, void * const funcPtr, const int varArgsCount, ...);
#endif

static void SetGLViewportSize(const int width, const int height);

static void OnFramebufferSizeChanged(GLFWwindow * const /*window*/, const int width, const int height);

static UniqueShader CompileShaderFromFile(const GLenum shaderType, const std::string & shaderSourceFilename);

static void OnKeyEvent(
    GLFWwindow * const window,
    const int          key,
    const int          scancode,
    const int          action,
    const int          mods
);

static void ProcessInput(GLFWwindow * const window);

static void TogglePolygonMode();

//
// Main
//

int main()
{
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

        BOOST_LOG_TRIVIAL(info)<< "Using OpenGL version " << glGetString(GL_VERSION);
        BOOST_LOG_TRIVIAL(info)<< "Using OpenGL renderer " << glGetString(GL_RENDERER);

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
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.8f, -0.8f, 0.0f,
            -0.6f, -0.6f, 0.0f,
            -0.7f, 0.2f, 0.0f
        };

        const std::vector<GLuint> indices0{
            0, 1, 2,
            1, 2, 3
        };

        // SECTION: VAO setup
        const std::vector<UniqueVAO>    vertexArrayObjects   = UniqueVAO::CreateMany(2);
        const std::vector<UniqueBuffer> vertexBufferObjects  = UniqueBuffer::CreateMany(2);

        const UniqueBuffer elementBufferObject0 = UniqueBuffer::Create();

        glBindVertexArray(vertexArrayObjects[0]);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[0]);
        glBufferData(GL_ARRAY_BUFFER, 4*3*sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject0);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(float), indices0.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), static_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glBindVertexArray(vertexArrayObjects[1]);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);
        glBufferData(GL_ARRAY_BUFFER, 3*3*sizeof(float), vertices.data() + 4*3, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), static_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glBindVertexArray(INVALID_OPENGL_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, INVALID_OPENGL_BUFFER);
        // END SECTION

        // SECTION: Shader program setup
        UniqueShader vertexShader   = CompileShaderFromFile(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE_FILENAME);

        const std::vector<UniqueShaderProgram> shaderPrograms = UniqueShaderProgram::CreateMany(2);

        for (size_t i = 0; i < shaderPrograms.size(); i++)
        {
            const UniqueShaderProgram & shaderProgram = shaderPrograms[i];

            assert(i < FRAGMENT_SHADER_SOURCE_FILENAMES.size());
            const std::string & fragmentShaderSourceFilename = FRAGMENT_SHADER_SOURCE_FILENAMES[i];

            UniqueShader fragmentShader = CompileShaderFromFile(GL_FRAGMENT_SHADER, fragmentShaderSourceFilename);

            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);

            {
                GLint linkStatusValue = GL_FALSE;
                glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatusValue);

                if (!linkStatusValue)
                {
                    static const size_t MAX_SHADER_LINKING_LOG_SIZE = 512;

                    std::string linkingLog(MAX_SHADER_LINKING_LOG_SIZE, '\0');
                    glGetShaderInfoLog(shaderProgram, MAX_SHADER_LINKING_LOG_SIZE, nullptr, linkingLog.data());

                    BOOST_LOG_TRIVIAL(fatal)<< "Failed to link shader program: " << linkingLog;

                    assert(false && "shader program linking must succeed");

                    // TODO: Replace with a custom exception
                    throw std::runtime_error("Failed to link shader program");
                }
            }

            BOOST_LOG_TRIVIAL(info)<< "Successfully linked shader program from "
                << VERTEX_SHADER_SOURCE_FILENAME << ", " << fragmentShaderSourceFilename;

            fragmentShader.Reset();
        }

        vertexShader.Reset();

        // END SECTION
        // END TODO0

        glfwSetKeyCallback(window.get(), &OnKeyEvent);

        while (!glfwWindowShouldClose(window.get()))
        {
            ProcessInput(window.get());

            // TODO: Extract as scene render logic
            glClearColor(0.5f, 0.75f, 0.75f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shaderPrograms[0]);
            glBindVertexArray(vertexArrayObjects[0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glUseProgram(shaderPrograms[1]);
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

#ifdef GLAD_DEBUG
static void OnGladFunctionCalled(const char * const funcName, void * const /*funcPtr*/, const int /*varArgsCount*/, ...)
{
    const GLenum errorCode = glad_glGetError();

    if (errorCode == GL_NO_ERROR)
        return;

    BOOST_LOG_TRIVIAL(error)<< "OpenGL error " << errorCode << " in " << funcName;

    assert(false && "OpenGL calls must succeed");
}
#endif

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

static UniqueShader CompileShaderFromFile(const GLenum shaderType, const std::string & shaderSourceFilename)
{
    const std::string shaderSource = ReadFileContent(SHADERS_DIR + shaderSourceFilename);
    BOOST_LOG_TRIVIAL(debug)<< "Loaded shader source from " << shaderSourceFilename << ":\n" << shaderSource;

    const char * const shaderSourceData = shaderSource.data();

    UniqueShader shader = UniqueShader::Create(shaderType);

    glShaderSource(shader, 1, &shaderSourceData, nullptr);
    glCompileShader(shader);

    {
        GLint compilationStatusValue = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationStatusValue);

        if (!compilationStatusValue)
        {
            static const size_t MAX_SHADER_COMPILATION_LOG_SIZE = 512;

            std::string compilationLog(MAX_SHADER_COMPILATION_LOG_SIZE, '\0');
            glGetShaderInfoLog(shader, MAX_SHADER_COMPILATION_LOG_SIZE, nullptr, compilationLog.data());

            BOOST_LOG_TRIVIAL(fatal)<< "Failed to compile shader from " << shaderSourceFilename << ": " << compilationLog;

            assert(false && "shader compilation must succeed");

            // TODO: Replace with a custom exception
            throw std::runtime_error("Failed to compile shader from " + shaderSourceFilename);
        }
    }

    return shader;
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

static void TogglePolygonMode()
{
  std::array<GLint, 2> polygonMode{{-1, -1}};

  // This seems to yield either 1 or 2 values (depending on the platform maybe?).
  // If there are 2 values, the first is for front face mode, the second - for back face.
  glGetIntegerv(GL_POLYGON_MODE, polygonMode.data());

  const GLint frontFaceMode = polygonMode[0];
  const GLint backFaceMode  = polygonMode[1];
  assert(frontFaceMode == backFaceMode || backFaceMode == -1);

  glPolygonMode(GL_FRONT_AND_BACK, frontFaceMode == GL_LINE ? GL_FILL : GL_LINE);
}

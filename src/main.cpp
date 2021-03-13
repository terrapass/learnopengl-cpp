#include <memory>
#include <cassert>
#include <vector>
#include <array>
#include <cmath>
#include <numbers>

#include <boost/format.hpp>
#include <boost/signals2.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "gl/constants.h"
#include "gl/wrappers.h"
#include "gl/utils.h"
#include "gl/shaders.h"
#include "gl/StatefulShaderProgram.h"
#include "meshes/construction.h"
#include "textures/loading.h"
#include "camera/Camera.h"
#include "camera/controllers.h"
#include "utils/boost_utils.h"
#include "utils/glfw_utils.h"
#include "config.h"
#include "logging.h"
#include "input.h"

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

static void OnKeyPressed(GLFWwindow * const window, const Key key);

static void SetMouseCursorCapture(GLFWwindow * const window, const bool isEnabled);

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

        boost::signals2::signal<void(float)> updateSignal;

        // SECTION: Input setup
        GlfwInputReceiver::InitializeInstance(window.get());
        GlfwInputReceiver::GetInstance()->KeyPressedSignal.connect(
            std::bind(&OnKeyPressed, window.get(), std::placeholders::_1)
        );
        // END SECTION

        // TODO0: Extract (scene setup logic, shader program loading) and refactor

        // SECTION: Camera setup
        static const LookAtSettings INITIAL_CAMERA_LOOK_AT_SETTINGS{
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        };

        static const PerspectiveProjection CAMERA_PROJECTION{
            0.5f * std::numbers::pi_v<float>,
            static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), // FIXME: Doesn't change on window resize
            0.1f,
            100.0f
        };

        Camera camera(INITIAL_CAMERA_LOOK_AT_SETTINGS, CAMERA_PROJECTION);

        FlyCameraController::Settings cameraControllerSettings;
        cameraControllerSettings.MovementSpeed       = 0.5f;
        cameraControllerSettings.RotationSensitivity = 0.025f;
        cameraControllerSettings.MustInvertPitch     = false;

        FlyCameraController cameraController(&camera, GlfwInputReceiver::GetInstance(), std::move(cameraControllerSettings));
        cameraController.SetEnabled(false);

        const boost::signals2::connection cameraControllerUpdateConnection = updateSignal.connect(
            std::bind(&decltype(cameraController)::Update, &cameraController, std::placeholders::_1)
        );

        // Toggle camera controller activation on RMB click
        GlfwInputReceiver::GetInstance()->MouseButtonPressedSignal.connect(
            [&cameraController, &window](
                const MouseButton button, const MouseState & /*mouseState*/
            ) {
                if (button != MouseButton::Right)
                    return;

                cameraController.SetEnabled(!cameraController.IsEnabled());

                SetMouseCursorCapture(window.get(), cameraController.IsEnabled());
            }
        );

        // END SECTION

        // SECTION: Mesh setup
        const Mesh cubeMesh = CreateUnitCubeMesh(false, true, false);
        // END SECTION

        // SECTION: Texture setup
        static const std::array TEXTURE_FILENAMES{
            "rtwe_output_cropped.jpeg"/*,
            "white_pawn.png"*/
        };

        const std::vector<UniqueTexture> textures = UniqueTexture::CreateMany(TEXTURE_FILENAMES.size());

        for (int textureIdx = 0; static_cast<size_t>(textureIdx) < textures.size(); textureIdx++)
        {
            const TextureData       textureData     = LoadTextureDataFromFile(TEXTURE_FILENAMES[textureIdx]);
            const TextureMetadata & textureMetadata = textureData.GetMetadata();

            glActiveTexture(GL_TEXTURE0 + textureIdx); // sic
            glBindTexture(GL_TEXTURE_2D, textures[textureIdx]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                textureMetadata.ChannelsCount == 3 ? GL_RGB : GL_RGBA,
                textureMetadata.Width,
                textureMetadata.Height,
                0,
                textureMetadata.ChannelsCount == 3 ? GL_RGB : GL_RGBA,
                GL_UNSIGNED_BYTE,
                textureData.GetData()
            );
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, INVALID_OPENGL_TEXTURE);
        // END SECTION

        // SECTION: Shader setup
        StatefulShaderProgram shaderProgram(MakeShaderProgramFromFilesPack(
            "basic_mvp.vert",
            "basic_texture.frag"
        ));
        //StatefulShaderProgram shaderProgram(MakeShaderProgramFromMatchingFiles("basic_texture"));

        shaderProgram.Use();

        shaderProgram.SetUniformValueByName("tex", 0); // Using GL_TEXTURE0 for this sampler uniform
        //shaderProgram.SetUniformValueByName("tex1", 1); // ...GL_TEXTURE1...

        const glm::mat4 & projection = camera.GetProjectionMatrix();

        shaderProgram.SetUniformValueByName("projection", projection);

        glUseProgram(INVALID_OPENGL_SHADER);

        const std::array cubePositions{
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(-3.0f, 3.0f, -1.0f),
            glm::vec3(2.0f, -3.0f, -2.0f),
            glm::vec3(0.5f, -2.0f, -1.0f),
            glm::vec3(1.5f, 2.0f, -5.0f)
        };
        // END SECTION

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        // END TODO0

        const uint64_t ticksPerSecond = glfwGetTimerFrequency();
        const float    secondsPerTick = 1.0f / static_cast<float>(ticksPerSecond);

        uint64_t lastTimeTicks = glfwGetTimerValue();

        while (!glfwWindowShouldClose(window.get()))
        {
            const uint64_t currentTimeTicks = glfwGetTimerValue();
            assert(currentTimeTicks >= lastTimeTicks);

            const uint64_t deltaTimeTicks   = currentTimeTicks - lastTimeTicks;
            const float    deltaTimeSeconds = secondsPerTick*deltaTimeTicks;

            lastTimeTicks = currentTimeTicks;

            updateSignal(deltaTimeSeconds);

            // TODO: Extract as scene render logic
            glClearColor(0.3f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shaderProgram.Use();

            const glm::mat4 & view = camera.GetLookAtMatrix();

            shaderProgram.SetUniformValueByName("view",  view);

            for (int textureIdx = 0; static_cast<size_t>(textureIdx) < textures.size(); textureIdx++)
            {
                glActiveTexture(GL_TEXTURE0 + textureIdx); // sic
                glBindTexture(GL_TEXTURE_2D, textures[textureIdx]);
            }

            cubeMesh.Bind();

            for (size_t i = 0; i < cubePositions.size(); i++)
            {
                const glm::vec3 & cubePosition = cubePositions[i];

                const glm::mat4 model = glm::rotate(
                    glm::translate(glm::mat4(1.0f), cubePosition),
                    static_cast<float>(i) + static_cast<float>(glfwGetTime()) / std::numbers::pi_v<float>,
                    glm::normalize(glm::vec3(0.5f, 1.0f, 0.0f))
                );

                shaderProgram.SetUniformValueByName("model", model);

                cubeMesh.Render(GL_TRIANGLES);
            }
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

static void OnKeyPressed(GLFWwindow * const window, const Key key)
{
    switch (key)
    {
    default:
        break;

    case Key::Escape:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;

    case Key::Z:
        TogglePolygonMode();
        break;
    }
}

static void SetMouseCursorCapture(GLFWwindow * const window, const bool isCaptureEnabled)
{
    return; // FIXME

    glfwSetInputMode(window, GLFW_CURSOR, isCaptureEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

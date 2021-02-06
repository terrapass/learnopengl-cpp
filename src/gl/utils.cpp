#include "utils.h"

#include <array>

#include "utils/file_utils.h"
#include "logging.h"
#include "config.h"

//
// Forward declarations
//

int GetGlIntegerParam(const GLenum name);

//
// Utilities
//

void LogGlInfo()
{
    BOOST_LOG_TRIVIAL(info)<< "Using OpenGL version " << glGetString(GL_VERSION);
    BOOST_LOG_TRIVIAL(info)<< "Using OpenGL renderer " << glGetString(GL_RENDERER);

    BOOST_LOG_TRIVIAL(info)<< "Max number of OpenGL vertex attributes: " << GetMaxVertexAttribs();
    BOOST_LOG_TRIVIAL(info)<< "Max OpenGL texture size: " << GetMaxTextureSize();
}

void SetViewportSize(const int width, const int height)
{
    assert(width > 0);
    assert(height > 0);

    glViewport(0, 0, width, height);

    BOOST_LOG_TRIVIAL(debug)<< "Set GL viewport size to " << width << 'x' << height;
}

void TogglePolygonMode()
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

UniqueShader CompileShaderFromFile(const GLenum shaderType, const std::string & shaderSourceFilename)
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

int GetMaxVertexAttribs()
{
    return GetGlIntegerParam(GL_MAX_VERTEX_ATTRIBS);
}

int GetMaxTextureSize()
{
    return GetGlIntegerParam(GL_MAX_TEXTURE_SIZE);
}

//
// Service
//

int GetGlIntegerParam(const GLenum name)
{
    static const int INVALID_PARAM_VALUE = -1;

    int result = INVALID_PARAM_VALUE;
    glGetIntegerv(name, &result);
    assert(result != INVALID_PARAM_VALUE);

    return result;
}

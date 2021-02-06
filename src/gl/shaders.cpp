#include "shaders.h"

#include <cassert>
#include <unordered_map>

#include "utils/file_utils.h"
#include "config.h"

//
// Constants
//

static const std::unordered_map<std::string, GLenum> SHADER_TYPES_BY_EXTENSION{
    {"vert", GL_VERTEX_SHADER},
    {"geom", GL_GEOMETRY_SHADER},
#ifdef GLAD_GL_VERSION_4_0
    {"tesc", GL_TESS_CONTROL_SHADER},
    {"tese", GL_TESS_EVALUATION_SHADER},
#endif
    {"frag", GL_FRAGMENT_SHADER},
#ifdef GLAD_GL_VERSION_4_3
    {"comp", GL_COMPUTE_SHADER}
#endif
};

//
// Forward declarations
//

static GLenum ExtensionToShaderType(const std::string & extension);

static const char * ShaderTypeToCStr(const GLenum shaderType);

//
// Utilities
//

GLenum DetermineShaderTypeFromFilename(const std::string & shaderSourceFilename)
{
    return ExtensionToShaderType(GetFileExtension(shaderSourceFilename));
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

            BOOST_LOG_TRIVIAL(fatal)<< "Failed to compile " << ShaderTypeToCStr(shaderType)
                << " shader " << shader << "from " << shaderSourceFilename << ": " << compilationLog;

            assert(false && "shader compilation must succeed");

            throw ShaderCompilationException(shaderType, shaderSourceFilename);
        }
    }

    BOOST_LOG_TRIVIAL(debug)<< "Successfully compiled " << ShaderTypeToCStr(shaderType)
        << " shader " << shader << " from " << shaderSourceFilename;

    return shader;
}

void LinkShaderProgram(const GLuint shaderProgram)
{
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

            throw ShaderProgramLinkingException();
        }
    }

    BOOST_LOG_TRIVIAL(debug)<< "Successfully linked shader program " << shaderProgram;
}

//
// Exceptions
//

ShaderCompilationException::ShaderCompilationException(
    const GLenum        shaderType,
    const std::string & shaderSourceFilename
):
    std::runtime_error("Failed to compile " + std::string(ShaderTypeToCStr(shaderType)) + " shader from " + shaderSourceFilename)
{
    // Empty
}

ShaderProgramLinkingException::ShaderProgramLinkingException():
    std::runtime_error("Failed to link shader program")
{
    // Empty
}

//
// Service
//

static GLenum ExtensionToShaderType(const std::string & extension)
{
    const auto ShaderTypeIt = SHADER_TYPES_BY_EXTENSION.find(extension);
    assert(ShaderTypeIt != SHADER_TYPES_BY_EXTENSION.cend());

    return ShaderTypeIt->second;
}

static const char * ShaderTypeToCStr(const GLenum shaderType)
{
    switch (shaderType)
    {
    case GL_VERTEX_SHADER:   return "vertex";
    case GL_GEOMETRY_SHADER: return "geometry";
#ifdef GLAD_GL_VERSION_4_0
    case GL_TESS_CONTROL_SHADER:    return "tesselation control";
    case GL_TESS_EVALUATION_SHADER: return "tesselation evaluation";
#endif
    case GL_FRAGMENT_SHADER: return "fragment";
#ifdef GLAD_GL_VERSION_4_3
    case GL_COMPUTE_SHADER: return "compute";
#endif
    }

    assert(false && "shader type must be valid and supported");
    return nullptr;
}

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

static std::string GetFullShaderPath(const std::string & shaderSourceFilename);

//
// Utilities
//

GLenum DetermineShaderTypeFromFilename(const std::string & shaderSourceFilename)
{
    return ExtensionToShaderType(GetFileExtension(shaderSourceFilename));
}

UniqueShader CompileShaderFromFile(const GLenum shaderType, const std::string & shaderSourceFilename)
{
    const std::string shaderSource = ReadFileContent(GetFullShaderPath(shaderSourceFilename));
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
                << " shader " << shader << " from " << shaderSourceFilename << ": " << compilationLog;

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

UniqueShaderProgram MakeShaderProgramFromFiles(const std::vector<std::string> & shaderSourceFilenames)
{
    assert(!shaderSourceFilenames.empty());

    UniqueShaderProgram shaderProgram = UniqueShaderProgram::Create();

    for (const std::string & shaderSourceFilename : shaderSourceFilenames)
    {
        const GLenum       shaderType = DetermineShaderTypeFromFilename(shaderSourceFilename);
        const UniqueShader shader     = CompileShaderFromFile(shaderType, shaderSourceFilename);

        glAttachShader(shaderProgram, shader);
    }

    LinkShaderProgram(shaderProgram);

    BOOST_LOG_TRIVIAL(info)<< "Successfully linked shader program " << shaderProgram << " from "
        << MakeCommaSeparatedList(shaderSourceFilenames);

    return shaderProgram;
}

UniqueShaderProgram MakeShaderProgramFromMatchingFiles(const std::string & matchingFilename)
{
    assert(!matchingFilename.empty());

    std::vector<std::string> shaderSourceFilenames;
    shaderSourceFilenames.reserve(SHADER_TYPES_BY_EXTENSION.size());

    for (const auto shaderTypeIt : SHADER_TYPES_BY_EXTENSION)
    {
        const std::string & fileExtension = shaderTypeIt.first;

        const std::string shaderSourceFilename = matchingFilename + '.' + fileExtension;

        if (DoesFileExist(GetFullShaderPath(shaderSourceFilename)))
            shaderSourceFilenames.push_back(shaderSourceFilename);
    }

    BOOST_LOG_TRIVIAL(info)<< "Found " << shaderSourceFilenames.size() << " matching shader files: "
        << MakeCommaSeparatedList(shaderSourceFilenames);

    return MakeShaderProgramFromFiles(shaderSourceFilenames);
}

GLuint GetCurrentlyUsedShaderProgram()
{
    GLint currentShaderProgram = -1;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentShaderProgram);
    assert(currentShaderProgram != -1);

    return static_cast<GLuint>(currentShaderProgram);
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
    const auto shaderTypeIt = SHADER_TYPES_BY_EXTENSION.find(extension);
    assert(shaderTypeIt != SHADER_TYPES_BY_EXTENSION.cend());

    return shaderTypeIt->second;
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

static std::string GetFullShaderPath(const std::string & shaderSourceFilename)
{
    assert(SHADERS_DIR.back() == '/' || SHADERS_DIR.empty());
    assert(!shaderSourceFilename.empty());

    return SHADERS_DIR + shaderSourceFilename;
}

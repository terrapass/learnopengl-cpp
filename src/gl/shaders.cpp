#include "shaders.h"

#include <cassert>

#include "utils/file_utils.h"
#include "config.h"
#include "logging.h"

//
// Utilities
//

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

            throw ShaderCompilationException(shaderType, shaderSourceFilename);
        }
    }

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
    const GLenum        /*shaderType*/, // TODO: Stringify and use in error message
    const std::string & shaderSourceFilename
):
    std::runtime_error("Failed to compile shader from " + shaderSourceFilename)
{
    // Empty
}

ShaderProgramLinkingException::ShaderProgramLinkingException():
    std::runtime_error("Failed to link shader program")
{
    // Empty
}

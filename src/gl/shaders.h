#pragma once

#include <stdexcept>

#include <glad/glad.h>

#include "utils/string_utils.h"
#include "logging.h"

#include "wrappers.h"

//
// Service
//

namespace detail
{

inline void AttachShaders(const GLuint /*shaderProgram*/)
{
    // Empty
}

template <typename... TailShaders>
inline void AttachShaders(const GLuint shaderProgram, const GLuint headShader, const TailShaders... tailShaders)
{
    glAttachShader(shaderProgram, headShader);

    AttachShaders(shaderProgram, tailShaders...);
}

} // namespace detail

//
// Utilities
//

GLenum DetermineShaderTypeFromFilename(const std::string & shaderSourceFilename);

UniqueShader CompileShaderFromFile(const GLenum shaderType, const std::string & shaderSourceFilename);

void LinkShaderProgram(const GLuint shaderProgram);

template <typename... Shader>
inline UniqueShaderProgram MakeShaderProgram(Shader &&... shaders)
{
    UniqueShaderProgram shaderProgram = UniqueShaderProgram::Create();

    detail::AttachShaders(shaderProgram, static_cast<GLuint>(shaders)...);
    LinkShaderProgram(shaderProgram);

    return shaderProgram;
}

template <typename... ShaderFileNames>
inline UniqueShaderProgram MakeShaderProgramFromFilesPack(ShaderFileNames &&... shaderSourceFilenames)
{
    static_assert(sizeof...(shaderSourceFilenames) > 0);

    UniqueShaderProgram shaderProgram = MakeShaderProgram(
        CompileShaderFromFile(
            DetermineShaderTypeFromFilename(shaderSourceFilenames),
            shaderSourceFilenames
        )...
    );

    BOOST_LOG_TRIVIAL(info)<< "Successfully linked shader program " << shaderProgram << " from "
        << MakeCommaSeparatedListFromPack(std::forward<ShaderFileNames>(shaderSourceFilenames)...);

    return shaderProgram;
}

UniqueShaderProgram MakeShaderProgramFromFiles(const std::vector<std::string> & shaderSourceFilenames);

UniqueShaderProgram MakeShaderProgramFromMatchingFiles(const std::string & matchingFilename);

//
// Exceptions
//

class ShaderCompilationException final: public std::runtime_error
{
public: // Construction

    explicit ShaderCompilationException(const GLenum shaderType, const std::string & shaderSourceFilename);
};

class ShaderProgramLinkingException final: public std::runtime_error
{
public: // Construction

    ShaderProgramLinkingException();
};

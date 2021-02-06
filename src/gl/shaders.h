#pragma once

#include <stdexcept>

#include <glad/glad.h>

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

//
// Exceptions
//

class ShaderCompilationException final: std::runtime_error
{
public: // Construction

    explicit ShaderCompilationException(const GLenum shaderType, const std::string & shaderSourceFilename);
};

class ShaderProgramLinkingException final: std::runtime_error
{
public: // Construction

    ShaderProgramLinkingException();
};

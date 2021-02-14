#pragma once

#include <vector>

#include <glad/glad.h>

namespace detail
{

struct VaoTraits final
{
    using ValueType = GLuint;

    static const char * const ValueTypeDisplayName;

    static GLuint Create();

    static std::vector<GLuint> CreateMany(const size_t count);

    static void Destroy(const GLuint vertexArrayObject);
};

struct BufferTraits final
{
    using ValueType = GLuint;

    static const char * const ValueTypeDisplayName;

    static GLuint Create();

    static std::vector<GLuint> CreateMany(const size_t count);

    static void Destroy(const GLuint vertexBufferObject);
};

struct ShaderTraits final
{
    using ValueType = GLuint;

    static const char * const ValueTypeDisplayName;

    static GLuint Create(const GLenum shaderType);

    static void Destroy(const GLuint shader);
};

struct ShaderProgramTraits final
{
    using ValueType = GLuint;

    static const char * const ValueTypeDisplayName;

    static GLuint Create();

    static void Destroy(const GLuint shaderProgram);
};

struct TextureTraits final
{
    using ValueType = GLuint;

    static const char * const ValueTypeDisplayName;

    static GLuint Create();

    static std::vector<GLuint> CreateMany(const size_t count);

    static void Destroy(const GLuint texture);
};

} // namespace detail

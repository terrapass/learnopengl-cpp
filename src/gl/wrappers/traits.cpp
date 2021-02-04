#include "traits.h"

#include <cassert>
#include <algorithm>

#include "gl/constants.h"

namespace detail
{

//
// VaoTraits
//

const char * const VaoTraits::ValueTypeDisplayName = "VAO";

GLuint VaoTraits::Create()
{
    GLuint vertexArrayObject = INVALID_OPENGL_VAO;
    glGenVertexArrays(1, &vertexArrayObject);
    assert(vertexArrayObject != INVALID_OPENGL_VAO);

    return vertexArrayObject;
}

std::vector<GLuint> VaoTraits::CreateMany(const size_t count)
{
    std::vector<GLuint> result(count, INVALID_OPENGL_VAO);
    assert(result.size() == count);

    glGenVertexArrays(static_cast<GLsizei>(count), result.data());

    assert(std::find(result.cbegin(), result.cend(), INVALID_OPENGL_VAO) == result.cend());
    return result;
}

void VaoTraits::Destroy(const GLuint vertexArrayObject)
{
    glDeleteVertexArrays(1, &vertexArrayObject);
}

//
// BufferTraits
//

const char * const BufferTraits::ValueTypeDisplayName = "buffer";

GLuint BufferTraits::Create()
{
    GLuint vertexBufferObject = INVALID_OPENGL_BUFFER;
    glGenBuffers(1, &vertexBufferObject);
    assert(vertexBufferObject != INVALID_OPENGL_BUFFER);

    return vertexBufferObject;
}

std::vector<GLuint> BufferTraits::CreateMany(const size_t count)
{
    std::vector<GLuint> result(count, INVALID_OPENGL_BUFFER);
    assert(result.size() == count);

    glGenBuffers(static_cast<GLsizei>(count), result.data());

    assert(std::find(result.cbegin(), result.cend(), INVALID_OPENGL_BUFFER) == result.cend());
    return result;
}

void BufferTraits::Destroy(const GLuint vertexBufferObject)
{
    glDeleteBuffers(1, &vertexBufferObject);
}

//
// ShaderTraits
//

const char * const ShaderTraits::ValueTypeDisplayName = "shader";

GLuint ShaderTraits::Create(const GLenum shaderType)
{
    const GLuint shader = glCreateShader(shaderType);
    assert(shader != INVALID_OPENGL_SHADER);

    return shader;
}

void ShaderTraits::Destroy(const GLuint shader)
{
    glDeleteShader(shader);
}

//
// ShaderProgramTraits
//

const char * const ShaderProgramTraits::ValueTypeDisplayName = "shader program";

GLuint ShaderProgramTraits::Create()
{
    const GLuint shaderProgram = glCreateProgram();
    assert(shaderProgram != INVALID_OPENGL_SHADER);

    return shaderProgram;
}

void ShaderProgramTraits::Destroy(const GLuint shaderProgram)
{
    glDeleteProgram(shaderProgram);
}

} // namespace detail

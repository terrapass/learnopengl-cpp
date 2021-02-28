#include "utils.h"

#include <array>

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

int GetMaxVertexAttribs()
{
    return GetGlIntegerParam(GL_MAX_VERTEX_ATTRIBS);
}

int GetMaxTextureSize()
{
    return GetGlIntegerParam(GL_MAX_TEXTURE_SIZE);
}

GLuint GetBoundVertexArray()
{
    return static_cast<GLuint>(GetGlIntegerParam(GL_VERTEX_ARRAY_BINDING));
}

GLuint GetBoundArrayBuffer()
{
    return static_cast<GLuint>(GetGlIntegerParam(GL_ARRAY_BUFFER_BINDING));
}

GLuint GetBoundElementArrayBuffer()
{
    return static_cast<GLuint>(GetGlIntegerParam(GL_ELEMENT_ARRAY_BUFFER_BINDING));
}

const char * GlErrorToCStr(const GLenum error)
{
    switch (error)
    {
    case GL_NO_ERROR:                      return "GL_NO_ERROR";
    case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
#ifdef GLAD_GL_VERSION_4_0
    case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
#endif
    case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    }

    assert(false && "unrecognized OpenGL error code");
    return "<UNRECOGNIZED>";
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

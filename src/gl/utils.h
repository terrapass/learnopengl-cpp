#pragma once

#include <glad/glad.h>

//
// Utilities
//

void LogGlInfo();

void SetViewportSize(const int width, const int height);

void TogglePolygonMode();

int GetMaxVertexAttribs();

int GetMaxTextureSize();

GLuint GetBoundVertexArray();

GLuint GetBoundArrayBuffer();

GLuint GetBoundElementArrayBuffer();

const char * GlErrorToCStr(const GLenum error);

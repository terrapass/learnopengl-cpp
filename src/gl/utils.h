#pragma once

#include <glad/glad.h>

#include "wrappers.h"

//
// Utilities
//

void SetViewportSize(const int width, const int height);

void TogglePolygonMode();

UniqueShader CompileShaderFromFile(const GLenum shaderType, const std::string & shaderSourceFilename);

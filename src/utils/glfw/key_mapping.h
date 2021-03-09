#pragma once

#include "input/Key.h"

//
// Utilities
//

Key KeyFromGlfw(const int glfwKey);

int KeyToGlfw(const Key key);

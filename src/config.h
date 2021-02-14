#pragma once

#include "logging.h"

//
// Constants
//

constexpr auto MIN_LOG_LEVEL = boost::log::trivial::debug; // TODO: Configure via CMake?

constexpr int OPENGL_MAJOR_VERSION = 3;
constexpr int OPENGL_MINOR_VERSION = 3;

const char * const WINDOW_TITLE = "learnopengl-cpp";

constexpr int WINDOW_WIDTH  = 800;
constexpr int WINDOW_HEIGHT = 600;

const std::string ASSETS_ROOT  = "assets/";
const std::string SHADERS_DIR  = ASSETS_ROOT + "shaders/";
const std::string TEXTURES_DIR = ASSETS_ROOT + "textures/";

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0048 NEW)
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW) # FindOpenGL will prefer GLVND
endif()

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find external packages

find_package(OpenGL REQUIRED) # OpenGL

# Select source files

set(SRC_FILES "src/glad.c")

# Configure target

add_library(glad_local ${SRC_FILES})

target_include_directories(
    glad_local
    PUBLIC
    "include"             # own header files
    ${OPENGL_INCLUDE_DIR} # OpenGL
)

target_link_libraries(
    glad_local
    OpenGL::GL # OpenGL
)


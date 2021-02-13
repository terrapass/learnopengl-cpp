cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0048 NEW) # Take values for VERSION variables from project()
project(learnopengl VERSION 0.0.1)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define user options

option(LEARNOPENGL_BUILD_GLFW "Build and use the embedded glfw version" ON)

# Setup paths to load cmake modules from
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Include dependencies

# Local GLAD
add_subdirectory("external/glad_local")

# glm
add_subdirectory("external/glm")

# glfw
if(LEARNOPENGL_BUILD_GLFW)
    message(STATUS "Will build and use the embedded glfw library version")

    add_subdirectory("external/glfw")
    set(GLFW_INCLUDE_DIRS "external/glfw/include")
endif()

# Find external packages

# glfw
if(NOT LEARNOPENGL_BUILD_GLFW)
    find_package(PkgConfig REQUIRED) # PkgConfig is needed to find the GLFW package.
    pkg_search_module(GLFW glfw3)

    if(NOT GLFW_FOUND)
        message(
            FATAL_ERROR 
            "Failed to find pre-installed GLFW.\nPlease set LEARNOPENGL_BUILD_GLFW to ON to use the embedded GLFW version."
        )
    else()
        message(STATUS "Will use pre-installed GLFW library ${GLFW_LIBRARY}")
    endif()
endif()

# Boost.Log
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS log REQUIRED)

# Select source files

file(
    GLOB_RECURSE
    LEARNOPENGL_SOURCES
    "src/*.cpp"
)

# Setup include directories

include_directories(
    "src"
    SYSTEM "external/glm"
    ${GLFW_INCLUDE_DIRS}
)

# Configure warnings for the following targets
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# learnopengl executable
add_executable(learnopengl ${LEARNOPENGL_SOURCES})
target_link_libraries(
    learnopengl
    glad_local
    glfw
    ${GLFW_LIBRARIES}
    Boost::log
)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0048 NEW)

# Select source files

set(SRC_FILES "stb_image.c")

# Configure target

add_library(stb_image_local ${SRC_FILES})

target_include_directories(
    stb_image_local
    PUBLIC
    "." # own header files
)

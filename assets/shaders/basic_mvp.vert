#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aRgb;
layout (location = 2) in vec2 aTextureUv;

out vec3 rgb;
out vec2 textureUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    rgb       = aRgb;
    textureUv = aTextureUv;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

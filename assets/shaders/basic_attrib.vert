#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aRgb;

out vec3 rgb;

void main()
{
    rgb = aRgb;

    gl_Position = vec4(aPos, 1.0);
}

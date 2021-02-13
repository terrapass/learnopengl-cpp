#version 330 core

in vec3 rgb;

out vec4 fragColor;

void main()
{
    fragColor = vec4(rgb, 1.0);
}
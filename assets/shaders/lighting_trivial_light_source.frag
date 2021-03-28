#version 330 core

out vec4 fragColor;

uniform vec3 lightRgb;

void main()
{
    fragColor = vec4(lightRgb, 1.0);
}

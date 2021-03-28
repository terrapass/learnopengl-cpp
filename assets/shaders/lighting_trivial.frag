#version 330 core

out vec4 fragColor;

uniform vec3 objectRgb;
uniform vec3 lightRgb;

void main()
{
    fragColor = vec4(lightRgb * objectRgb, 1.0);
}

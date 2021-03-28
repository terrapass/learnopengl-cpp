#version 330 core

out vec4 fragColor;

uniform vec3 objectRgb;
uniform vec3 lightRgb;

uniform float ambientStrength;

void main()
{
    vec3 ambientRgb = ambientStrength*lightRgb;

    fragColor = vec4(ambientRgb * objectRgb, 1.0);
}

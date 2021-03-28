#version 330 core

in vec3 worldPos;
in vec3 normal;

out vec4 fragColor;

uniform vec3 lightSourcePosition;

uniform vec3 objectRgb;
uniform vec3 lightRgb;

uniform float ambientStrength;

void main()
{
    vec3 ambientRgb = ambientStrength*lightRgb;

    vec3  reverseLightDirection = normalize(lightSourcePosition - worldPos);
    float cosTheta              = dot(normal, reverseLightDirection);
    float diffuseStrength       = max(cosTheta, 0.0f);
    vec3  diffuseRgb            = diffuseStrength*objectRgb;

    fragColor = vec4((ambientRgb + diffuseRgb) * objectRgb, 1.0);
}

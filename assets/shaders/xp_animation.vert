#version 330 core

const float PI = 3.1415926538;

const float ALPHA = 1.0f;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aRgb;

out vec3 rgb;

uniform float elapsedSeconds;

mat4 rotationAxis(float angle, vec3 axis)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 rotateY(vec3 p, float angle)
{
    mat4 rmy = rotationAxis(angle, vec3(0.0, 1.0, 0.0));
    return (vec4(p, 1.0) * rmy).xyz;
}

vec3 rotateZ(vec3 p, float angle)
{
    mat4 rmy = rotationAxis(angle, vec3(0.0, 0.0, 1.0));
    return (vec4(p, 1.0) * rmy).xyz;
}

void main()
{
    float redFactor   = 0.5f + 0.5f*sin(elapsedSeconds);
    float greenFactor = 0.5f + 0.5f*cos(2.5f*elapsedSeconds);
    float blueFactor  = 0.5f + 0.5f*sin(0.33f*PI - 3.3f*elapsedSeconds);

    rgb = vec3(redFactor*aRgb.x, greenFactor*aRgb.y, blueFactor*aRgb.z);

    float scale  = 0.75f + 0.25f*sin(0.5f*PI + elapsedSeconds);
    float angleY = elapsedSeconds;
    float angleZ = 0.5f*elapsedSeconds;

    vec3 pos = scale*rotateY(rotateZ(aPos, angleZ), angleY);

    gl_Position = vec4(pos, 1.0);
}

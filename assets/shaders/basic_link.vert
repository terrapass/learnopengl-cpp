#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 vertexColor;

void main()
{
    vec3 normPos   = (aPos + 1.0f) / 2.0f;
    vec3 vertexRgb = normPos / max(normPos.x, max(normPos.y, normPos.z));

    vertexColor = vec4(vertexRgb, 1.0f);

    gl_Position = vec4(aPos, 1.0f);
}

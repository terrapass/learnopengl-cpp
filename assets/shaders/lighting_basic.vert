#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aRgb;
layout (location = 2) in vec2 aTextureUv;
layout (location = 3) in vec3 aNormal;

out vec3 worldPos;
out vec3 rgb;
out vec2 textureUv;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    worldPos  = vec3(model * vec4(aPos, 1.0));
    rgb       = aRgb;
    textureUv = aTextureUv;
    normal    = aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#version 330 core

in vec3 rgb;
in vec2 textureUv;

out vec4 fragColor;

uniform sampler2D tex;

void main()
{
    vec4 texColor = texture(tex, textureUv);

    fragColor = vec4(rgb, 1.0) * texColor;
}

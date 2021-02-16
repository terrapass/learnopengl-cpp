#version 330 core

// Compatible with basic_texture.vert

in vec3 rgb;
in vec2 textureUv;

out vec4 fragColor;

uniform float textureMixAmount;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{
    //fragColor = mix(texture(tex0, textureUv), texture(tex1, textureUv), textureMixAmount);
    fragColor = (1.0 - textureMixAmount)*texture(tex0, textureUv) + textureMixAmount*texture(tex1, textureUv);
}

#version 330 core

in vec3 rgb;

out vec4 fragColor;

uniform float displayWidth;
uniform float displayHeight;
uniform float minAlphaFalloffRadius;
uniform float maxAlphaFalloffRadius;

uniform float elapsedSeconds;

void main()
{
    float alphaFalloffRadiusRange = maxAlphaFalloffRadius - minAlphaFalloffRadius;

    vec2 falloffOrigin = vec2(
        0.5f*displayWidth + sin(elapsedSeconds)*0.25f*displayWidth,
        0.5f*displayHeight + cos(elapsedSeconds)*0.25f*displayHeight
    );

    float distanceFromFalloffOrigin = distance(gl_FragCoord.xy, falloffOrigin);

    float alpha = 1.0f - clamp(
        (distanceFromFalloffOrigin - minAlphaFalloffRadius) / alphaFalloffRadiusRange,
        0.0f,
        1.0f
    );

    fragColor = vec4(rgb, alpha);
}

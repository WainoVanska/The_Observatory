#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform float brightnessThreshold;

void main()
{
    vec3 hdrColor = texture(
        sceneTexture,
        TexCoords
    ).rgb;

    float brightness = dot(
        hdrColor,
        vec3(0.2126, 0.7152, 0.0722)
    );

    if (brightness > brightnessThreshold)
    {
        FragColor = vec4(hdrColor, 1.0);
    }
    else
    {
        FragColor = vec4(0.0);
    }
}
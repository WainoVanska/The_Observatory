#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool blurEnabled;

void main()
{
    if (!blurEnabled)
    {
        FragColor = texture(screenTexture, TexCoords);
        return;
    }

    float offset = 1.0 / 300.0;

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0,     offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0),
        vec2( 0.0,     0.0),
        vec2( offset,  0.0),
        vec2(-offset, -offset),
        vec2( 0.0,    -offset),
        vec2( offset, -offset)
    );

    float kernel[9] = float[](
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
    );

    vec3 color = vec3(0.0);

    for (int i = 0; i < 9; i++)
    {
        color += vec3(texture(screenTexture, TexCoords + offsets[i])) * kernel[i];
    }

    FragColor = vec4(color, 1.0);
}
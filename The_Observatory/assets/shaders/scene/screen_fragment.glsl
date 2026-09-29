#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform bool bloomEnabled;
uniform float bloomStrength;
uniform bool blurEnabled;
uniform bool gammaCorrectionEnabled;
uniform bool hdrEnabled;
uniform float exposure;
uniform sampler2D depthTexture;
uniform bool depthDebugEnabled;
uniform float nearPlane;
uniform float farPlane;

float linearizeDepth(float depth)
{
    float ndcDepth = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - ndcDepth * (farPlane - nearPlane));
}

void main()
{
    if (depthDebugEnabled)
    {
        float storedDepth = texture(depthTexture, TexCoords).r;

        float linearDepth = linearizeDepth(storedDepth);

        float normalizedDepth = log(1.0 + linearDepth) / log(1.0 + farPlane);

        float depthColor = 1.0 - clamp(normalizedDepth, 0.0, 1.0);

        FragColor = vec4(vec3(depthColor), 1.0);

        return;
    }

    vec3 color;

    if (!blurEnabled)
    {
        color = texture(screenTexture, TexCoords).rgb;
    }
    else
    {
        float offset = 1.0 / 300.0;

        // clang-format off
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
        // clang-format on

        color = vec3(0.0);

        for (int i = 0; i < 9; i++)
        {
            color += texture(screenTexture, TexCoords + offsets[i]).rgb * kernel[i];
        }
    }

    if (bloomEnabled)
    {
        vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;

        color += bloomColor * bloomStrength;
    }

    if (hdrEnabled)
    {
        color = vec3(1.0) - exp(-color * exposure);
    }

    if (gammaCorrectionEnabled)
    {
        const float gamma = 2.2;

        color = pow(max(color, vec3(0.0)), vec3(1.0 / gamma));
    }

    FragColor = vec4(color, 1.0);
}

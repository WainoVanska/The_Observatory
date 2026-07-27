#version 330 core

out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 GeometricNormal;
    mat3 TBN;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform vec2 uvScale;
uniform float normalStrength;

uniform samplerCube pointShadowMap;
uniform float pointShadowFarPlane;
uniform bool pointShadowsEnabled;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform bool lightEnabled;

const vec3 pointShadowSampleOffsets[20] = vec3[]
(
    vec3( 1.0,  1.0,  1.0),
    vec3( 1.0, -1.0,  1.0),
    vec3(-1.0, -1.0,  1.0),
    vec3(-1.0,  1.0,  1.0),

    vec3( 1.0,  1.0, -1.0),
    vec3( 1.0, -1.0, -1.0),
    vec3(-1.0, -1.0, -1.0),
    vec3(-1.0,  1.0, -1.0),

    vec3( 1.0,  1.0,  0.0),
    vec3( 1.0, -1.0,  0.0),
    vec3(-1.0, -1.0,  0.0),
    vec3(-1.0,  1.0,  0.0),

    vec3( 1.0,  0.0,  1.0),
    vec3(-1.0,  0.0,  1.0),
    vec3( 1.0,  0.0, -1.0),
    vec3(-1.0,  0.0, -1.0),

    vec3( 0.0,  1.0,  1.0),
    vec3( 0.0, -1.0,  1.0),
    vec3( 0.0, -1.0, -1.0),
    vec3( 0.0,  1.0, -1.0)
);

float calculatePointShadow(
    vec3 fragmentPosition,
    vec3 pointLightPosition,
    vec3 geometricNormal
)
{
    if (!pointShadowsEnabled)
    {
        return 0.0;
    }

    vec3 fragToLight =
        fragmentPosition -
        pointLightPosition;

    float currentDepth =
        length(fragToLight);

    if (currentDepth >= pointShadowFarPlane)
    {
        return 0.0;
    }

    vec3 lightDir =
        normalize(
            pointLightPosition -
            fragmentPosition
        );

    float bias =
        max(
            0.035 *
            (
                1.0 -
                dot(
                    geometricNormal,
                    lightDir
                )
            ),
            0.012
        );

    float viewDistance =
        length(
            viewPos -
            fragmentPosition
        );

    float diskRadius =
        (
            1.0 +
            viewDistance /
            pointShadowFarPlane
        ) /
        90.0;

    float shadow = 0.0;

    const int sampleCount = 20;

    for (
        int sampleIndex = 0;
        sampleIndex < sampleCount;
        ++sampleIndex
    )
    {
        float closestDepth =
            texture(
                pointShadowMap,
                fragToLight +
                pointShadowSampleOffsets[
                    sampleIndex
                ] *
                diskRadius
            ).r;

        closestDepth *=
            pointShadowFarPlane;

        if (
            currentDepth - bias >
            closestDepth
        )
        {
            shadow += 1.0;
        }
    }

    return shadow /
        float(sampleCount);
}

void main()
{
    vec2 tiledTexCoords =
        fs_in.TexCoords * uvScale;

    vec3 baseColor =
        texture(
            diffuseMap,
            tiledTexCoords
        ).rgb;

    vec3 tangentNormal =
        texture(
            normalMap,
            tiledTexCoords
        ).rgb;

    tangentNormal =
        tangentNormal * 2.0 - 1.0;

    tangentNormal.xy *=
        normalStrength;

    tangentNormal =
        normalize(tangentNormal);

    vec3 mappedNormal =
        normalize(
            fs_in.TBN *
            tangentNormal
        );

    vec3 geometricNormal =
        normalize(
            fs_in.GeometricNormal
        );

    vec3 result =
        0.015 *
        baseColor;

    if (lightEnabled)
    {
        vec3 lightDir =
            normalize(
                lightPos -
                fs_in.FragPos
            );

        float diffuseStrength =
            max(
                dot(
                    mappedNormal,
                    lightDir
                ),
                0.0
            );

        vec3 diffuse =
            diffuseStrength *
            lightColor *
            baseColor;

        vec3 viewDir =
            normalize(
                viewPos -
                fs_in.FragPos
            );

        vec3 halfwayDir =
            normalize(
                lightDir +
                viewDir
            );

        float specularStrength =
            pow(
                max(
                    dot(
                        mappedNormal,
                        halfwayDir
                    ),
                    0.0
                ),
                32.0
            );

        vec3 specular =
            0.15 *
            specularStrength *
            lightColor;

        float distanceToLight =
            length(
                lightPos -
                fs_in.FragPos
            );

        float attenuation =
            1.0 /
            (
                1.0 +
                0.14 *
                distanceToLight +
                0.07 *
                distanceToLight *
                distanceToLight
            );
        
        float shadow =
        calculatePointShadow(
            fs_in.FragPos,
            lightPos,
            geometricNormal
        );

        result +=
            attenuation *
            (1.0 - shadow) *
            (
                diffuse +
                specular
            );
    }

    FragColor =
        vec4(result, 1.0);
}
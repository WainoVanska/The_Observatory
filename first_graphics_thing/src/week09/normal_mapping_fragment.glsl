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

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform bool lightEnabled;

void main()
{
    vec3 baseColor =
        texture(
            diffuseMap,
            fs_in.TexCoords
        ).rgb;

    /*
     * Normal-map values are stored in [0, 1].
     * Convert them back into tangent-space
     * directions in [-1, 1]
     */
    vec3 tangentNormal =
        texture(
            normalMap,
            fs_in.TexCoords
        ).rgb;

    tangentNormal =
        tangentNormal *
        2.0 -
        1.0;

    vec3 mappedNormal =
        normalize(
            fs_in.TBN *
            tangentNormal
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

        result +=
            attenuation *
            (
                diffuse +
                specular
            );
    }

    FragColor =
        vec4(result, 1.0);
}
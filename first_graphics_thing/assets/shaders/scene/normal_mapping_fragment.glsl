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
uniform sampler2D depthMap;

uniform vec2 uvScale;
uniform float normalStrength;
uniform float heightScale;

uniform samplerCube pointShadowMap0;
uniform samplerCube pointShadowMap1;
uniform samplerCube pointShadowMap2;
uniform samplerCube pointShadowMap3;
uniform float pointShadowFarPlane;
uniform bool pointShadowsEnabled;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform vec3 flashlightPos;
uniform vec3 flashlightDir;
uniform bool flashlightEnabled;
uniform float flashlightCutOff;
uniform float flashlightOuterCutOff;

uniform vec3 wallLightPos;
uniform vec3 wallLightColor;
uniform bool wallLightEnabled;

uniform bool lightEnabled;

const int POINT_LIGHT_COUNT = 4;

struct PointLight
{
    vec3 position;
    vec3 color;
    bool enabled;
};

uniform PointLight pointLights[POINT_LIGHT_COUNT];

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

float samplePointShadowMap(
    int lightIndex,
    vec3 sampleDirection
)
{
    if (lightIndex == 0)
    {
        return texture(
            pointShadowMap0,
            sampleDirection
        ).r;
    }

    if (lightIndex == 1)
    {
        return texture(
            pointShadowMap1,
            sampleDirection
        ).r;
    }

    if (lightIndex == 2)
    {
        return texture(
            pointShadowMap2,
            sampleDirection
        ).r;
    }

    return texture(
        pointShadowMap3,
        sampleDirection
    ).r;
}

float calculatePointShadow(
    vec3 fragmentPosition,
    vec3 pointLightPosition,
    vec3 geometricNormal,
    int lightIndex
)
{
    if (!pointShadowsEnabled)
    {
        return 0.0;
    }

    vec3 fragToLight = fragmentPosition - pointLightPosition;
    float currentDepth = length(fragToLight);

    if (currentDepth >= pointShadowFarPlane)
    {
        return 0.0;
    }

    vec3 lightDir = normalize(pointLightPosition - fragmentPosition);

    float bias = max(
        0.035 * (1.0 - dot(geometricNormal, lightDir)),
        0.012
    );

    float viewDistance = length(viewPos - fragmentPosition);
    float diskRadius = (1.0 + viewDistance / pointShadowFarPlane) / 90.0;

    float shadow = 0.0;
    const int sampleCount = 20;

    for (int sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        vec3 sampleDirection =
            fragToLight +
            pointShadowSampleOffsets[sampleIndex] * diskRadius;

        float closestDepth =
            samplePointShadowMap(
                lightIndex,
                sampleDirection
            );
        closestDepth *= pointShadowFarPlane;

        if (currentDepth - bias > closestDepth)
        {
            shadow += 1.0;
        }
    }

    return shadow / float(sampleCount);
}

float sampleDepthMap(vec2 texCoords)
{
    return 1.0 - texture(depthMap, texCoords).r;
}

vec2 parallaxOcclusionMapping(vec2 texCoords, vec3 tangentViewDir)
{
    if (heightScale <= 0.0 || tangentViewDir.z <= 0.0)
    {
        return texCoords;
    }

    const float minLayers = 16.0;
    const float maxLayers = 64.0;

    float viewAlignment = clamp(tangentViewDir.z, 0.0, 1.0);
    float numLayers = mix(maxLayers, minLayers, viewAlignment);
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    float safeViewZ = max(tangentViewDir.z, 0.05);
    vec2 totalTexCoordOffset = tangentViewDir.xy / safeViewZ * heightScale;
    vec2 deltaTexCoords = totalTexCoordOffset / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = sampleDepthMap(currentTexCoords);

    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = sampleDepthMap(currentTexCoords);
        currentLayerDepth += layerDepth;
    }

    vec2 previousTexCoords = currentTexCoords + deltaTexCoords;

    float depthAfterIntersection = currentDepthMapValue - currentLayerDepth;
    float depthBeforeIntersection =
        sampleDepthMap(previousTexCoords) - currentLayerDepth + layerDepth;

    float denominator = depthAfterIntersection - depthBeforeIntersection;

    float weight = abs(denominator) > 0.00001
        ? depthAfterIntersection / denominator
        : 0.0;

    weight = clamp(weight, 0.0, 1.0);

    return mix(currentTexCoords, previousTexCoords, weight);
}

vec3 calculateRoomPointLight(
    PointLight pointLight,
    int lightIndex,
    vec3 mappedNormal,
    vec3 geometricNormal,
    vec3 viewDirection,
    vec3 baseColor)
{
    vec3 lightDir = normalize(
        pointLight.position -
        fs_in.FragPos
    );

    vec3 halfwayDir = normalize(
        lightDir +
        viewDirection
    );

    float diffuseStrength = max(
        dot(mappedNormal, lightDir),
        0.0
    );

    vec3 diffuse =
        diffuseStrength *
        pointLight.color *
        baseColor;

    float specularStrength = pow(
        max(
            dot(mappedNormal, halfwayDir),
            0.0
        ),
        32.0
    );

    vec3 specular =
        0.15 *
        specularStrength *
        pointLight.color;

    float distanceToLight = length(
        pointLight.position -
        fs_in.FragPos
    );

    float attenuation = 1.0 / (
        1.0 +
        0.14 * distanceToLight +
        0.07 *
        distanceToLight *
        distanceToLight
    );

    float shadow = 0.0;

    if (pointShadowsEnabled)
    {
        shadow = calculatePointShadow(
            fs_in.FragPos,
            pointLight.position,
            geometricNormal,
            lightIndex
        );
    }

    return attenuation *
        (1.0 - shadow) *
        (diffuse + specular);
}

vec3 calculateFlashlight(
    vec3 mappedNormal,
    vec3 viewDirection,
    vec3 baseColor
)
{
    vec3 lightDir = normalize(
        flashlightPos - fs_in.FragPos
    );

    float theta = dot(
        lightDir,
        normalize(-flashlightDir)
    );

    float epsilon = max(
        flashlightCutOff - flashlightOuterCutOff,
        0.0001
    );

    float rawIntensity = clamp(
        (theta - flashlightOuterCutOff) / epsilon,
        0.0,
        1.0
    );

    float intensity = smoothstep(
        0.0,
        1.0,
        rawIntensity
    );

    float diffuseStrength = max(
        dot(mappedNormal, lightDir),
        0.0
    );

    vec3 flashlightColor = vec3(
        2.5,
        2.3,
        2.0
    );

    vec3 diffuse =
        diffuseStrength *
        flashlightColor *
        baseColor;

    vec3 halfwayDir = normalize(
        lightDir + viewDirection
    );

    float specularStrength = pow(
        max(dot(mappedNormal, halfwayDir), 0.0),
        32.0
    );

    vec3 specular =
        0.15 *
        specularStrength *
        flashlightColor;

    float distanceToLight = length(
        flashlightPos - fs_in.FragPos
    );

    float attenuation = 1.0 / (
        1.0 +
        0.25 * distanceToLight +
        0.20 *
        distanceToLight *
        distanceToLight
    );

    return attenuation *
        intensity *
        (diffuse + specular);
}

void main()
{
    vec2 tiledTexCoords = fs_in.TexCoords * uvScale;

    vec3 worldViewDir = normalize(viewPos - fs_in.FragPos);
    vec3 tangentViewDir = normalize(transpose(fs_in.TBN) * worldViewDir);

    tiledTexCoords =
        parallaxOcclusionMapping(
            tiledTexCoords,
            tangentViewDir
        );

    vec3 baseColor = texture(diffuseMap, tiledTexCoords).rgb;
    vec3 tangentNormal = texture(normalMap, tiledTexCoords).rgb;

    tangentNormal = tangentNormal * 2.0 - 1.0;
    tangentNormal.xy *= normalStrength;
    tangentNormal = normalize(tangentNormal);

    vec3 mappedNormal = normalize(fs_in.TBN * tangentNormal);
    vec3 geometricNormal = normalize(fs_in.GeometricNormal);

    vec3 result = 0.015 * baseColor;

    for (int i = 0; i < POINT_LIGHT_COUNT; ++i)
    {
        if (!pointLights[i].enabled)
        {
            continue;
        }

        result += calculateRoomPointLight(
            pointLights[i],
            i,
            mappedNormal,
            geometricNormal,
            worldViewDir,
            baseColor
        );
    }

    if (flashlightEnabled)
    {
        result += calculateFlashlight(
            mappedNormal,
            worldViewDir,
            baseColor
        );
    }

    FragColor = vec4(result, 1.0);
}
#version 330 core

in vec3 ourColor;
in vec2 ourTexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 secondLightPos;
uniform vec3 secondLightColor;
uniform vec3 viewPos;

uniform vec3 flashlightPos;
uniform vec3 flashlightDir;

uniform bool flashlightEnabled;

uniform float flashlightCutOff;
uniform float flashlightOuterCutOff;

uniform bool light1Enabled;
uniform bool light2Enabled;

const int POINT_LIGHT_COUNT = 4;

struct PointLight
{
    vec3 position;
    vec3 color;
    bool enabled;
};

uniform PointLight pointLights[POINT_LIGHT_COUNT];

uniform bool directionalLightEnabled;
uniform vec3 directionalLightDirection;
uniform vec3 directionalLightColor;

uniform samplerCube pointShadowMap0;
uniform samplerCube pointShadowMap1;
uniform samplerCube pointShadowMap2;
uniform samplerCube pointShadowMap3;
uniform float pointShadowFarPlane;
uniform bool pointShadowsEnabled;

// clang-format off
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
// clang-format on

float samplePointShadowMap(int lightIndex, vec3 sampleDirection)
{
    if (lightIndex == 0)
    {
        return texture(pointShadowMap0, sampleDirection).r;
    }

    if (lightIndex == 1)
    {
        return texture(pointShadowMap1, sampleDirection).r;
    }

    if (lightIndex == 2)
    {
        return texture(pointShadowMap2, sampleDirection).r;
    }

    return texture(pointShadowMap3, sampleDirection).r;
}

uniform sampler2D directionalShadowMap;

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;

    vec3 diffuseColor;
    vec3 specularColor;

    float shininess;

    bool useDiffuseMap;
    bool useSpecularMap;
};

uniform Material material;

vec3 getBaseColor()
{
    if (material.useDiffuseMap)
    {
        return texture(material.diffuseMap, ourTexCoord).rgb;
    }

    return material.diffuseColor;
}

vec3 getSpecularColor()
{
    if (material.useSpecularMap)
    {
        return texture(material.specularMap, ourTexCoord).rgb;
    }

    return material.specularColor;
}

float calculatePointShadow(vec3 fragmentPosition, vec3 pointLightPosition, vec3 norm, int lightIndex)
{
    if (!pointShadowsEnabled)
    {
        return 0.0;
    }

    vec3 fragToLight = fragmentPosition - pointLightPosition;

    float currentDepth = length(fragToLight);

    vec3 lightDir = normalize(pointLightPosition - fragmentPosition);

    float bias = max(0.035 * (1.0 - dot(norm, lightDir)), 0.012);

    float viewDistance = length(viewPos - fragmentPosition);

    float diskRadius = (1.0 + viewDistance / pointShadowFarPlane) / 90.0;

    float shadow = 0.0;

    const int sampleCount = 20;

    for (int sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        float closestDepth =
            samplePointShadowMap(lightIndex, fragToLight + pointShadowSampleOffsets[sampleIndex] * diskRadius);

        closestDepth *= pointShadowFarPlane;

        if (currentDepth - bias > closestDepth)
        {
            shadow += 1.0;
        }
    }

    return shadow / float(sampleCount);
}

vec3 calculatePointLight(vec3 currentLightPos,
                         vec3 currentLightColor,
                         vec3 norm,
                         vec3 viewDir,
                         bool castsShadow,
                         int shadowMapIndex)
{
    vec3 baseColor = getBaseColor();
    vec3 specularColor = getSpecularColor();

    vec3 lightDir = normalize(currentLightPos - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * currentLightColor * baseColor;

    // ambience
    vec3 ambient = 0.02 * currentLightColor * baseColor;

    // specular, Blinn-Phong version
    float specularStrength = 1.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * currentLightColor * specularColor;

    // attenuation / distance falloff
    float distanceToLight = length(currentLightPos - FragPos);

    float constant = 1.0;
    float linear = 0.14;
    float quadratic = 0.07;

    float attenuation = 1.0 / (constant + linear * distanceToLight + quadratic * distanceToLight * distanceToLight);

    float shadow = 0.0;

    if (castsShadow)
    {
        shadow = calculatePointShadow(FragPos, currentLightPos, norm, shadowMapIndex);
    }

    return attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 calculateFlashlight(vec3 norm, vec3 viewDir)
{
    vec3 baseColor = getBaseColor();
    vec3 specularColor = getSpecularColor();

    vec3 lightDir = normalize(flashlightPos - FragPos);

    float theta = dot(lightDir, normalize(-flashlightDir));
    float epsilon = flashlightCutOff - flashlightOuterCutOff;
    float rawIntensity = clamp((theta - flashlightOuterCutOff) / epsilon, 0.0, 1.0);
    float intensity = smoothstep(0.0, 1.0, rawIntensity);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 flashlightColor = vec3(2.5, 2.3, 2.0);
    vec3 diffuse = diff * flashlightColor * baseColor;

    // ambient
    float ambientStrength = 0.001;
    vec3 ambient = ambientStrength * baseColor;

    // specular
    float specularStrength = 0.7;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * flashlightColor * specularColor;

    // attenuation
    float distanceToLight = length(flashlightPos - FragPos);

    float constant = 1.0;
    float linear = 0.25;
    float quadratic = 0.20;

    float attenuation = 1.0 / (constant + linear * distanceToLight + quadratic * distanceToLight * distanceToLight);

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}

float calculateDirectionalShadow(vec4 fragPosLightSpace, vec3 norm, vec3 lightDir)
{
    // Convert from homogeneous clip coordinates.
    vec3 projectedCoordinates = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Convert from [-1, 1] to [0, 1].
    projectedCoordinates = projectedCoordinates * 0.5 + 0.5;

    // The fragment lies outside the directional light's far plane.
    if (projectedCoordinates.z > 1.0)
    {
        return 0.0;
    }

    float currentDepth = projectedCoordinates.z;

    float bias = max(0.005 * (1.0 - dot(norm, lightDir)), 0.0005);

    vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);

    float shadow = 0.0;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float closestDepth = texture(directionalShadowMap, projectedCoordinates.xy + vec2(x, y) * texelSize).r;

            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}

vec3 calculateDirectionalLight(vec3 lightDirection, vec3 currentLightColor, vec3 norm, vec3 viewDir)
{
    vec3 baseColor = getBaseColor();

    vec3 specularColor = getSpecularColor();

    vec3 lightDir = normalize(-lightDirection);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * currentLightColor * baseColor;

    float ambientStrength = 0.18;

    vec3 ambient = ambientStrength * currentLightColor * baseColor;

    float specularStrength = 0.4;

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);

    vec3 specular = specularStrength * spec * currentLightColor * specularColor;

    float shadow = calculateDirectionalShadow(FragPosLightSpace, norm, lightDir);

    return ambient + (1.0 - shadow) * (diffuse + specular);
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    // Four room lights
    for (int i = 0; i < POINT_LIGHT_COUNT; ++i)
    {
        if (!pointLights[i].enabled)
        {
            continue;
        }

        result +=
            calculatePointLight(pointLights[i].position, pointLights[i].color, norm, viewDir, pointShadowsEnabled, i);
    }

    // Blue point light does not use a shadow cubemap
    if (light2Enabled)
    {
        result += calculatePointLight(secondLightPos, secondLightColor, norm, viewDir, false, 0);
    }

    if (flashlightEnabled)
    {
        result += calculateFlashlight(norm, viewDir);
    }

    if (directionalLightEnabled)
    {
        result += calculateDirectionalLight(directionalLightDirection, directionalLightColor, norm, viewDir);
    }

    FragColor = vec4(result, 1.0);
}

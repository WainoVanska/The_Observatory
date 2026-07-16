#version 330 core

in vec3 ourColor;
in vec2 ourTexCoord;
in vec3 Normal;
in vec3 FragPos;

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

uniform bool directionalLightEnabled;
uniform vec3 directionalLightDirection;
uniform vec3 directionalLightColor;

struct Material {
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

vec3 calculatePointLight(
    vec3 currentLightPos,
    vec3 currentLightColor,
    vec3 norm,
    vec3 viewDir
)
{
    vec3 baseColor = getBaseColor();
    vec3 specularColor = getSpecularColor();

    vec3 lightDir = normalize(currentLightPos - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * currentLightColor * baseColor;

    //ambience
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

    float attenuation = 1.0 / (
        constant +
        linear * distanceToLight +
        quadratic * distanceToLight * distanceToLight
    );

    diffuse *= attenuation;
    specular *= attenuation;

    return attenuation * (
        ambient +
        diffuse +
        specular
    );
}

vec3 calculateFlashlight(
    vec3 norm,
    vec3 viewDir
)
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

    float attenuation = 1.0 / (
        constant +
        linear * distanceToLight +
        quadratic * distanceToLight * distanceToLight
    );

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}

vec3 calculateDirectionalLight(
    vec3 lightDirection,
    vec3 currentLightColor,
    vec3 norm,
    vec3 viewDir
)
{
    vec3 baseColor = getBaseColor();
    vec3 specularColor = getSpecularColor();

    // Direction from fragment toward the light
    vec3 lightDir = normalize(-lightDirection);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * currentLightColor * baseColor;

    float ambientStrength = 0.18;
    vec3 ambient = ambientStrength * currentLightColor * baseColor;

    float specularStrength = 0.4;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * currentLightColor * specularColor;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    // first point light
    if (light1Enabled)
    {
        result += calculatePointLight(lightPos, lightColor, norm, viewDir);
    }

    // second point light
    if (light2Enabled)
    {
        result += calculatePointLight(secondLightPos, secondLightColor, norm, viewDir);
    }

    if (flashlightEnabled)
    {
        result += calculateFlashlight(norm, viewDir);
    }

    if (directionalLightEnabled)
    {
        result += calculateDirectionalLight(
            directionalLightDirection,
            directionalLightColor,
            norm,
            viewDir
        );
    }

    FragColor = vec4(result, 1.0);
}
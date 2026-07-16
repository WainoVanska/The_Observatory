#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 secondLightPos;
uniform vec3 secondLightColor;

uniform bool light1Enabled;
uniform bool light2Enabled;

uniform bool directionalLightEnabled;
uniform vec3 directionalLightDirection;
uniform vec3 directionalLightColor;

uniform bool flashlightEnabled;
uniform vec3 flashlightPos;
uniform vec3 flashlightDir;
uniform float flashlightCutOff;
uniform float flashlightOuterCutOff;

uniform vec3 objectColor;
uniform vec3 objectSpecularColor;
uniform float shininess;

vec3 safeNormalize(
    vec3 value,
    vec3 fallback
)
{
    float squaredLength = dot(value, value);

    if (squaredLength < 0.000001)
    {
        return fallback;
    }

    return value * inversesqrt(squaredLength);
}

vec3 calculatePointLight(
    vec3 currentLightPos,
    vec3 currentLightColor,
    vec3 norm,
    vec3 viewDir
)
{
    vec3 lightDir = safeNormalize(
        currentLightPos - FragPos,
        vec3(0.0, 1.0, 0.0)
    );

    float diffuseAmount =
        max(dot(norm, lightDir), 0.0);

    vec3 diffuse =
        diffuseAmount *
        currentLightColor *
        objectColor;

    vec3 halfwayDir = safeNormalize(
        lightDir + viewDir,
        lightDir
    );

    float specularAmount = pow(
        max(dot(norm, halfwayDir), 0.0),
        shininess
    );

    vec3 specular =
        specularAmount *
        currentLightColor *
        objectSpecularColor;

    float distanceToLight =
        length(currentLightPos - FragPos);

    float constant = 1.0;
    float linear = 0.14;
    float quadratic = 0.07;

    float attenuation = 1.0 / (
        constant +
        linear * distanceToLight +
        quadratic * distanceToLight * distanceToLight
    );

    return attenuation * (
        diffuse +
        specular
    );
}

vec3 calculateDirectionalLight(
    vec3 norm,
    vec3 viewDir
)
{
    vec3 lightDir = safeNormalize(
        -directionalLightDirection,
        vec3(0.0, 1.0, 0.0)
    );

    float diffuseAmount =
        max(dot(norm, lightDir), 0.0);

    vec3 diffuse =
        diffuseAmount *
        directionalLightColor *
        objectColor;

    vec3 halfwayDir = safeNormalize(
        lightDir + viewDir,
        lightDir
    );

    float specularAmount = pow(
        max(dot(norm, halfwayDir), 0.0),
        shininess
    );

    vec3 specular =
        0.4 *
        specularAmount *
        directionalLightColor *
        objectSpecularColor;

    return diffuse + specular;
}

vec3 calculateFlashlight(
    vec3 norm,
    vec3 viewDir
)
{
    vec3 lightDir = safeNormalize(
        flashlightPos - FragPos,
        vec3(0.0, 1.0, 0.0)
    );

    float theta = dot(
        lightDir,
        safeNormalize(
            -flashlightDir,
            vec3(0.0, 0.0, -1.0)
        )
    );

    float epsilon =
        flashlightCutOff -
        flashlightOuterCutOff;

    float intensity = clamp(
        (theta - flashlightOuterCutOff) /
        max(epsilon, 0.0001),
        0.0,
        1.0
    );

    float diffuseAmount =
        max(dot(norm, lightDir), 0.0);

    vec3 flashlightColor =
        vec3(2.5, 2.3, 2.0);

    vec3 diffuse =
        diffuseAmount *
        flashlightColor *
        objectColor;

    vec3 halfwayDir = safeNormalize(
        lightDir + viewDir,
        lightDir
    );

    float specularAmount = pow(
        max(dot(norm, halfwayDir), 0.0),
        shininess
    );

    vec3 specular =
        0.7 *
        specularAmount *
        flashlightColor *
        objectSpecularColor;

    float distanceToLight =
        length(flashlightPos - FragPos);

    float attenuation = 1.0 / (
        1.0 +
        0.25 * distanceToLight +
        0.20 * distanceToLight * distanceToLight
    );

    return intensity *
        attenuation *
        (diffuse + specular);
}

void main()
{
    vec3 norm = safeNormalize(
        Normal,
        vec3(0.0, 1.0, 0.0)
    );

    vec3 viewDir = safeNormalize(
        viewPos - FragPos,
        vec3(0.0, 0.0, 1.0)
    );

    // The rocks remain visible even when every optional light is off.
    vec3 result = vec3(0.0);

    if (light1Enabled)
    {
        result += calculatePointLight(
            lightPos,
            lightColor,
            norm,
            viewDir
        );
    }

    if (light2Enabled)
    {
        result += calculatePointLight(
            secondLightPos,
            secondLightColor,
            norm,
            viewDir
        );
    }

    if (directionalLightEnabled)
    {
        result += calculateDirectionalLight(
            norm,
            viewDir
        );
    }

    if (flashlightEnabled)
    {
        result += calculateFlashlight(
            norm,
            viewDir
        );
    }

    FragColor = vec4(result, 1.0);
}
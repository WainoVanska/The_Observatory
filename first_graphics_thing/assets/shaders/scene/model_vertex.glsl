#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 secondLightPos;
uniform vec3 secondLightColor;
uniform vec3 viewPos;

uniform vec3 flashlightPos;
uniform vec3 flashlightDir;

uniform bool flashlightEnabled;
uniform bool light1Enabled;
uniform bool light2Enabled;

uniform bool directionalLightEnabled;
uniform vec3 directionalLightDirection;
uniform vec3 directionalLightColor;

uniform float flashlightCutOff;
uniform float flashlightOuterCutOff;

uniform samplerCube skybox;
uniform bool glassBackpackEnabled;

uniform samplerCube pointShadowMap;
uniform float pointShadowFarPlane;
uniform bool pointShadowsEnabled;

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

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

uniform Material material;

vec3 getBaseColor()
{
    return texture(
        material.texture_diffuse1,
        TexCoords
    ).rgb;
}

vec3 getSpecularColor()
{
    return texture(
        material.texture_specular1,
        TexCoords
    ).rgb;
}

float calculatePointShadow(
    vec3 fragmentPosition,
    vec3 pointLightPosition,
    vec3 norm
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

    float bias = max(
        0.035 *
        (1.0 - dot(norm, lightDir)),
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

    for (int sampleIndex = 0;
         sampleIndex < sampleCount;
         ++sampleIndex)
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

vec3 calculatePointLight(
    vec3 currentLightPos,
    vec3 currentLightColor,
    vec3 norm,
    vec3 viewDir,
    bool castsShadow
)
{
    vec3 baseColor = getBaseColor();
    vec3 specularColor = getSpecularColor();

    vec3 lightDir = normalize(currentLightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * currentLightColor * baseColor;

    vec3 ambient = 0.02 * currentLightColor * baseColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * currentLightColor * specularColor;

    float distanceToLight = length(currentLightPos - FragPos);

    float constant = 1.0;
    float linear = 0.14;
    float quadratic = 0.07;

    float attenuation = 1.0 / (
        constant +
        linear * distanceToLight +
        quadratic * distanceToLight * distanceToLight
    );

    float shadow = 0.0;

    if (
        castsShadow &&
        pointShadowsEnabled
    )
    {
        shadow = calculatePointShadow(
            FragPos,
            currentLightPos,
            norm
        );
    }

    return attenuation *
    (
        ambient +
        (1.0 - shadow) *
        (
            diffuse +
            specular
        )
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

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 flashlightColor = vec3(2.5, 2.3, 2.0);
    vec3 diffuse = diff * flashlightColor * baseColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = 0.7 * spec * flashlightColor * specularColor;

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

    vec3 ambient = 0.001 * baseColor;

    return ambient +
        attenuation *
        intensity *
        (diffuse + specular);
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

    // lightDirection describes where the sunlight travels toward
    vec3 lightDir = normalize(-lightDirection);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * currentLightColor * baseColor;

    float specularStrength = 0.4;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * currentLightColor * specularColor;

    vec3 ambient = 0.18 * currentLightColor * baseColor;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    if (glassBackpackEnabled)
    {
        float ratio = 1.00 / 1.52;
        vec3 I = normalize(FragPos - viewPos);
        vec3 R = refract(I, norm, ratio);

        FragColor = vec4(
            texture(skybox, R).rgb,
            1.0
        );

        return;
    }

    vec3 result = vec3(0.0);    

    if (light1Enabled)
    {
        result += calculatePointLight(
            lightPos,
            lightColor,
            norm,
            viewDir,
            true
        );
    }

    if (light2Enabled)
    {
        result += calculatePointLight(
            secondLightPos,
            secondLightColor,
            norm,
            viewDir,
            false
        );
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

    if (flashlightEnabled)
    {
        result += calculateFlashlight(
            norm,
            viewDir
        );
    }

    FragColor = vec4(result, 1.0);
}
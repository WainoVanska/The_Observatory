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

uniform bool light1Enabled;
uniform bool light2Enabled;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

vec3 calculatePointLight(
    vec3 currentLightPos,
    vec3 currentLightColor,
    vec3 norm,
    vec3 viewDir
)
{
    vec3 lightDir = normalize(currentLightPos - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * currentLightColor * material.diffuse;

    // ambient
    float ambientStrength = 0.12;
    vec3 ambient = ambientStrength * currentLightColor * material.ambient;

    // specular, Blinn-Phong version
    float specularStrength = 0.2;
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * currentLightColor * material.specular;

    // attenuation / distance falloff
    float distanceToLight = length(currentLightPos - FragPos);

    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;

    float attenuation = 1.0 / (
        constant +
        linear * distanceToLight +
        quadratic * distanceToLight * distanceToLight
    );

    diffuse *= attenuation;
    specular *= attenuation;

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

    FragColor = vec4(result, 1.0);
}
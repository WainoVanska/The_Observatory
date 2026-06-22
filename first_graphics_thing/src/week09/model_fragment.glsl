#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

uniform Material material;

void main()
{
    vec3 baseColor = texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 specularMap = texture(material.texture_specular1, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    float ambientStrength = 0.12;
    vec3 ambient = ambientStrength * lightColor * baseColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * lightColor * specularMap;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
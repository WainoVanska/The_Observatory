#version 330 core

in vec3 ourColor;
in vec2 ourTexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform float shininess;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor * material.diffuse;

	// ambient
	float ambientStrength = 0.12;
	vec3 ambient = ambientStrength * lightColor * material.ambient;

	// specular
	float specularStrength = 0.2;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
	vec3 specular = specularStrength * spec * lightColor * material.specular;

	// attenuation / distance falloff
	float distanceToLight = length(lightPos - FragPos);

	float constant = 1.0;
	float linear = 0.09;
	float quadratic = 0.032;

	float attenuation = 1.0 / (constant + linear * distanceToLight + quadratic * distanceToLight * distanceToLight);

	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0);
}
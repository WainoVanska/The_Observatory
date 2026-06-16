#version 330 core

in vec3 ourColor;
in vec2 ourTexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// ambient
	float ambientStrength = 0.12;
	vec3 ambient = ambientStrength * lightColor;

	// specular
	float specularStrength = 0.2;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	// attenuation / distance falloff
	float distanceToLight = length(lightPos - FragPos);

	float constant = 1.0;
	float linear = 0.09;
	float quadratic = 0.032;

	float attenuation = 1.0 / (constant + linear * distanceToLight + quadratic * distanceToLight * distanceToLight);

	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = (ambient + diffuse + specular) * objectColor;

	FragColor = vec4(result, 1.0);
}
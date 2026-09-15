#version 330 core

in vec3 ourColor;
in vec2 ourTexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture1;     // looks inside texture1 at UV coordinates ourTexCoord, get pixel from there and use as a color
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 texColor = texture(texture1, ourTexCoord).rgb;

    vec3 ambient = 0.15 * lightColor;
    vec3 result = (ambient + diffuse) * texColor;

    FragColor = vec4(result, 1.0);
}